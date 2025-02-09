// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once
#include "primitive.hpp"

#include "openvino/core/strides.hpp"
#include "openvino/core/coordinate_diff.hpp"
#include <vector>

namespace cldnn {

/// @brief Performs transposed convolution.
/// Also supports built-in Relu @ref activation available by setting it in arguments.
/// @details Deconvolution is similar to convolution layer with the weights flipped on the axis
/// and stride and input padding parameters used in opposite sense as in convolution.
struct deconvolution : public primitive_base<deconvolution> {
    CLDNN_DECLARE_PRIMITIVE(deconvolution)
    /// @brief Constructs deconvolution primitive.
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param bias List of primitive ids containing bias data. Provide empty vector if using next parameters without bias.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  const std::vector<primitive_id>& bias,
                  ov::Strides stride = {1, 1},
                  ov::CoordinateDiff pad = {0, 0},
                  ov::Strides dilations = {1, 1},
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(false),
          groups(1),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(false),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(bias) {}
    /// @brief Constructs deconvolution primitive.
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param groups Number of filter groups.
    /// @param weights List of primitive ids containing weights data.
    /// @param bias List of primitive ids containing bias data. Provide empty vector if using next parameters without bias.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  const std::vector<primitive_id>& bias,
                  uint32_t groups,
                  ov::Strides stride = {1, 1},
                  ov::CoordinateDiff pad = {0, 0},
                  ov::Strides dilations = {1, 1},
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(false),
          groups(groups),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(false),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(bias) {}

    /// @brief Constructs deconvolution primitive (w/o bias).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  ov::Strides stride = {1, 1},
                  ov::CoordinateDiff pad = {0, 0},
                  ov::Strides dilations = {1, 1},
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(false),
          groups(1),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(false),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(std::vector<primitive_id>(0)) {}

    /// @brief Constructs deconvolution primitive (w/o bias).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param groups Number of filter groups.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id> &weights,
                  uint32_t groups,
                  ov::Strides stride = {1, 1},
                  ov::CoordinateDiff pad = {0, 0},
                  ov::Strides dilations = {1, 1},
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(false),
          groups(groups),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(false),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(std::vector<primitive_id>(0)) {}

    /// @brief Constructs deconvolution primitive (computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param bias List of primitive ids containing bias data. Provide empty vector if using next parameters without bias.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  const std::vector<primitive_id>& bias,
                  ov::Strides stride,
                  ov::CoordinateDiff pad,
                  ov::Strides dilations,
                  tensor output_size,
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(true),
          output_size(output_size),
          groups(1),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(false),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(bias) {}

    /// @brief Constructs deconvolution primitive (computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param bias List of primitive ids containing bias data. Provide empty vector if using next parameters without bias.
    /// @param groups Number of filter groups.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  const std::vector<primitive_id>& bias,
                  uint32_t groups,
                  ov::Strides stride,
                  ov::CoordinateDiff pad,
                  ov::Strides dilations,
                  tensor output_size,
                  bool grouped_weights_shape,
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(true),
          output_size(output_size),
          groups(groups),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(grouped_weights_shape),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(bias) {}

    /// @brief Constructs deconvolution primitive with dynamic shape.
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param bias List of primitive ids containing bias data. Provide empty vector if using next parameters without bias.
    /// @param groups Number of filter groups.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  const std::vector<primitive_id>& bias,
                  uint32_t groups,
                  ov::Strides stride,
                  ov::CoordinateDiff pad,
                  ov::Strides dilations,
                  ov::CoordinateDiff pads_begin,
                  ov::CoordinateDiff pads_end,
                  ov::CoordinateDiff out_padding,
                  bool grouped_weights_shape,
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(false),
          groups(groups),
          pads_begin(pads_begin),
          pads_end(pads_end),
          out_padding(out_padding),
          grouped_weights_shape(grouped_weights_shape),
          output_partial_shape({}),
          output_shape_id(""),
          weights(weights),
          bias(bias) {}

    /// @brief Constructs deconvolution primitive (w/o bias, computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    deconvolution(const primitive_id& id,
                  const input_info& input,
                  const std::vector<primitive_id>& weights,
                  ov::Strides stride,
                  ov::CoordinateDiff pad,
                  ov::Strides dilations,
                  tensor output_size,
                  const padding& output_padding = padding())
        : primitive_base(id, {input}, {output_padding}),
          pad(pad),
          stride(stride),
          dilations(dilations),
          with_output_size(true),
          output_size(output_size),
          groups(1),
          pads_begin(pad.size(), 0),
          pads_end(pad.size(), 0),
          out_padding(pad.size(), 0),
          grouped_weights_shape(false),
          weights(weights),
          bias(std::vector<primitive_id>(0)) {}

    /// @brief Constructs deconvolution primitive (computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param bias List of primitive ids containing bias data. Provide empty vector if using next parameters without bias.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    /// @return Deconvolution primitive with specified settings.
    static deconvolution create_with_output_size(const primitive_id& id,
                                                 const input_info& input,
                                                 const std::vector<primitive_id>& weights,
                                                 const std::vector<primitive_id>& bias,
                                                 tensor output_size,
                                                 ov::Strides stride = {1, 1},
                                                 ov::CoordinateDiff pad = {0, 0},
                                                 ov::Strides dilations = {1, 1},
                                                 const padding& output_padding = padding()) {
        return deconvolution(id,
                             input,
                             weights,
                             bias,
                             stride,
                             pad,
                             dilations,
                             output_size,
                             output_padding);
    }

    /// @brief Constructs deconvolution primitive (w/o bias; computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param weights List of primitive ids containing weights data.
    /// @param pad Defines logical pad value added to input tensor
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param with_activation Enables Relu activation.
    /// @param activation_slp Relu activation slope.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    /// @return Deconvolution primitive with specified settings.
    static deconvolution create_with_output_size(const primitive_id& id,
                                                 const input_info& input,
                                                 const std::vector<primitive_id>& weights,
                                                 tensor output_size,
                                                 ov::Strides stride = {1, 1},
                                                 ov::CoordinateDiff pad = {0, 0},
                                                 ov::Strides dilations = {1, 1},
                                                 const padding& output_padding = padding())     {
        return deconvolution(id,
                             input,
                             weights,
                             stride,
                             pad,
                             dilations,
                             output_size,
                             output_padding);
    }

    /// @brief Defines logical pad value added to input tensor.
    ov::CoordinateDiff pad;
    /// @brief Defines shift in input buffer between adjacent calculations of output values.
    ov::Strides stride;
    /// @brief Defines the distance in width and height between elements in the filter.
    ov::Strides dilations;
    /// @brief Indicates that the primitive has user-defined output size (non-zero value).
    bool with_output_size;
    /// @brief User-defined output data size of the primitive (w/o padding).
    tensor output_size;
    /// @brief Number of feature groups (grouped convolution). If more than 1 then weights/bias count needs to be 1.
    uint32_t groups;
    /// @brief Defines a padding added to input image on left (x axis) and top (y axis).
    ov::CoordinateDiff pads_begin;
    /// @brief Defines a padding added to input image on right (x axis) and bottom (y axis).
    ov::CoordinateDiff pads_end;
    /// @brief Defines additional amount of paddings per each spatial axis added to output tensor.
    ov::CoordinateDiff out_padding;
    /// @param grouped_weights_shape Defines if weights tensor has explicit group dimension.
    bool grouped_weights_shape;
    /// @brief Defines spatial shape of the output.
    ov::PartialShape output_partial_shape;
    /// @brief Data primitive id containing spatial shape of the output.
    primitive_id output_shape_id;
    /// @brief List of primitive ids containing weights data.
    const primitive_id_arr weights;
    /// @brief List of primitive ids containing bias data.
    const primitive_id_arr bias;


protected:
    std::vector<std::reference_wrapper<const primitive_id>> get_dependencies() const override {
        std::vector<std::reference_wrapper<const primitive_id>> ret;
        ret.reserve(weights.size() + bias.size() + (output_shape_id.empty() ? 0 : 1));
        for (auto& w : weights) ret.push_back(std::ref(w));
        for (auto& b : bias) ret.push_back(std::ref(b));
        if (!output_shape_id.empty()) ret.push_back(output_shape_id);

        return ret;
    }
};
}  // namespace cldnn
