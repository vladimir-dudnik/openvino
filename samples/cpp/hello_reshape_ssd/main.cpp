// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <memory>
#include <sstream>
#include <string>
#include <vector>

// clang-format off
#include "ngraph/ngraph.hpp"
#include "openvino/openvino.hpp"

#include "samples/args_helper.hpp"
#include "samples/common.hpp"
#include "samples/slog.hpp"
#include "format_reader_ptr.h"
#include "reshape_ssd_extension.hpp"
// clang-format on

using namespace ov::preprocess;

int main(int argc, char* argv[]) {
    try {
        // -------- Get OpenVINO runtime version --------
        slog::info << *ov::get_openvino_version() << slog::endl;

        // --------------------------- Parsing and validation of input arguments
        if (argc != 5) {
            std::cout << "Usage : " << argv[0] << " <path_to_model> <path_to_image> <device> <batch>" << std::endl;
            return EXIT_FAILURE;
        }
        const std::string model_path{argv[1]};
        const std::string image_path{argv[2]};
        const std::string device_name{argv[3]};
        const size_t batch_size{std::stoul(argv[4])};
        // -----------------------------------------------------------------------------------------------------

        // Step 1. Initialize inference engine core
        ov::runtime::Core core;
        // -----------------------------------------------------------------------------------------------------

        // Step 2. Read a model
        slog::info << "Loading model files: " << model_path << slog::endl;
        auto model = core.read_model(model_path);
        printInputAndOutputsInfo(*model);

        OPENVINO_ASSERT(model->get_parameters().size() == 1, "Sample supports models with 1 input only");
        OPENVINO_ASSERT(model->get_results().size() == 1, "Sample supports models with 1 output only");
        // -----------------------------------------------------------------------------------------------------

        // Step 3. Configure input & output

        // Read input image without resize
        FormatReader::ReaderPtr reader(image_path.c_str());
        if (reader.get() == nullptr) {
            std::cout << "Image " + image_path + " cannot be read!" << std::endl;
            return 1;
        }

        std::shared_ptr<unsigned char> image_data = reader->getData();
        size_t image_channels = 3;
        size_t image_width = reader->width();
        size_t image_height = reader->height();

        // reshape model to image size and batch size
        ov::Shape tensor_shape = model->input().get_shape();

        tensor_shape[0] = batch_size;
        tensor_shape[1] = image_channels;
        tensor_shape[2] = image_height;
        tensor_shape[3] = image_width;

        std::cout << "Reshape network to the image size = [" << image_height << "x" << image_width << "] "
                  << "with batch = " << batch_size << std::endl;
        model->reshape({{model->input().get_any_name(), tensor_shape}});
        printInputAndOutputsInfo(*model);

        // Step 4. Apply preprocessing
        const ov::Layout tensor_layout{"NHWC"};

        // clang-format off
        model = PrePostProcessor(model).
            // 1) InputInfo() with no args assumes a model has a single input
            input(InputInfo().
                // 2) Set input tensor information:
                // - precision of tensor is supposed to be 'u8'
                // - layout of data is 'NHWC'
                // - set static spatial dimensions to input tensor to resize from
                tensor(InputTensorInfo().
                    set_element_type(ov::element::f32).
                    set_spatial_static_shape(
                        tensor_shape[ov::layout::height_idx(tensor_layout)],
                        tensor_shape[ov::layout::width_idx(tensor_layout)]).
                    set_layout(tensor_layout)).
                // 3) Adding explicit preprocessing steps:
                // - convert u8 to f32
                // - convert layout to 'NCHW' (from 'NHWC' specified above at tensor layout)
                preprocess(PreProcessSteps().
                    convert_element_type(ov::element::f32).
                    convert_layout("NCHW")).
                // 4) Here we suppose model has 'NCHW' layout for input
                network(InputNetworkInfo().
                    set_layout("NCHW"))).
            output(OutputInfo().
                tensor(OutputTensorInfo().
                    set_element_type(ov::element::f32))).
        // 6) Apply preprocessing modifing the original 'model'
        build();
        // clang-format on

        // Step 5. Loading a model to the device
        // -------------------------------------------------
        ov::runtime::ExecutableNetwork executable_network = core.compile_model(model, device_name);

        // Step 6. Create an infer request
        ov::runtime::InferRequest infer_request = executable_network.create_infer_request();

        // Step 7. Prepare input
        ov::runtime::Tensor input_tensor = infer_request.get_input_tensor();

        // copy NHWC data from image to tensor with batch
        unsigned char* image_data_ptr = image_data.get();
        unsigned char* tensor_data_ptr = input_tensor.data<unsigned char>();
        size_t image_size = image_width * image_height * image_channels;
        for (size_t b = 0; b < batch_size; b++) {
            for (size_t i = 0; i < image_size; i++) {
                tensor_data_ptr[b * image_size + i] = image_data_ptr[i];
            }
        }

        // Step 8. Do inference synchronously
        infer_request.infer();

        // Step 9. Process output
        ov::runtime::Tensor output_tensor = infer_request.get_output_tensor();

        ov::Shape output_shape = model->output().get_shape();
        const size_t max_proposal_count = output_shape[2];
        const size_t object_size = output_shape[3];

        const float* detection = output_tensor.data<const float>();

        std::vector<std::vector<int>> boxes(batch_size);
        std::vector<std::vector<int>> classes(batch_size);

        // Each detection has image_id that denotes processed image
        for (size_t cur_proposal = 0; cur_proposal < max_proposal_count; cur_proposal++) {
            auto image_id = static_cast<int>(detection[cur_proposal * object_size + 0]);
            if (image_id < 0) {
                break;
            }

            float confidence = detection[cur_proposal * object_size + 2];
            auto label = static_cast<int>(detection[cur_proposal * object_size + 1]);
            auto xmin = static_cast<int>(detection[cur_proposal * object_size + 3] * image_width);
            auto ymin = static_cast<int>(detection[cur_proposal * object_size + 4] * image_height);
            auto xmax = static_cast<int>(detection[cur_proposal * object_size + 5] * image_width);
            auto ymax = static_cast<int>(detection[cur_proposal * object_size + 6] * image_height);

            if (confidence > 0.5f) {
                // Drawing only objects with >50% probability
                classes[image_id].push_back(label);
                boxes[image_id].push_back(xmin);
                boxes[image_id].push_back(ymin);
                boxes[image_id].push_back(xmax - xmin);
                boxes[image_id].push_back(ymax - ymin);

                std::cout << "[" << cur_proposal << "," << label << "] element, prob = " << confidence << "    ("
                          << xmin << "," << ymin << ")-(" << xmax << "," << ymax << ")"
                          << " batch id = " << image_id;
                std::cout << std::endl;
            }
        }

        for (size_t batch_id = 0; batch_id < batch_size; ++batch_id) {
            addRectangles(image_data.get(),
                          image_height,
                          image_width,
                          boxes[batch_id],
                          classes[batch_id],
                          BBOX_THICKNESS);
            std::stringstream os;
            os << "hello_reshape_ssd_batch_" << batch_id << ".bmp";
            const std::string image_path = os.str();
            if (writeOutputBmp(image_path, image_data.get(), image_height, image_width)) {
                std::cout << "The resulting image was saved in the file: " + image_path << std::endl;
            } else {
                throw std::logic_error(std::string("Can't create a file: ") + image_path);
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
