// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vector>

#include "low_precision_transformations/multiply_transformation.hpp"
#include "common_test_utils/test_constants.hpp"

using namespace LayerTestsDefinitions;

namespace {
const std::vector<ngraph::element::Type> netPrecisions = {
    ngraph::element::f32,
    //ngraph::element::f16
};

// If snippets fuse all operations into one subgraph node,
// it's impossible to extract exec precision for the specific layer
const auto precision_for_fused_cases = ov::element::undefined;

const std::vector<LayerTestsDefinitions::MultiplyTestValues> params = {
    {
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        false,
        { 256ul, ngraph::Shape {}, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        precision_for_fused_cases,
        true
    },
    {
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        precision_for_fused_cases,
        false
    },
    {
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        precision_for_fused_cases,
        false
    },
    {
        true,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        precision_for_fused_cases,
        false
    },
    {
        true,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        precision_for_fused_cases,
        false
    },
    {
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        true,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -1.28f }, { 1.27f } },
        precision_for_fused_cases,
        false
    },
    {
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.28f }, { 1.27f }, { -128.f }, { 1.27f } },
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        precision_for_fused_cases,
        false
    },
    {
        false,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        true,
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { -1.27f }, { 1.28f }, { -1.27f }, { 1.28f } },
        { 256ul, ngraph::Shape { 1, 1, 1, 1 }, { 0.f }, { 2.55f }, { 0.f }, { 2.55f } },
        precision_for_fused_cases,
        false
    },
    { false, {}, false, {}, {}, ngraph::element::f32, false },
    { true, {}, true, {}, {}, ngraph::element::f32, false },
};

INSTANTIATE_TEST_SUITE_P(smoke_LPT, MultiplyTransformation,
    ::testing::Combine(
        ::testing::ValuesIn(netPrecisions),
        ::testing::Values(ngraph::PartialShape({ 1, 3, 16, 16 })),
        ::testing::Values(CommonTestUtils::DEVICE_CPU),
        ::testing::ValuesIn(params)),
    MultiplyTransformation::getTestCaseName);
}  // namespace



