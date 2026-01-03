/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/enum_flags.hpp"
#include "common/struct_type_macro.hpp"

#include "gc/type.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

namespace sieve {

enum class ImageMetric : uint8_t
{
    StateHistogram,
    EdgeHistogram
};

using ImageMetricSet = common::EnumFlags<ImageMetric>;

struct ImageMetrics
{
    std::vector<double> histogram;
    std::vector<double> edge_histogram;
};
GCLIB_STRUCT_TYPE(ImageMetrics, histogram, edge_histogram);

auto operator<<(std::ostream&, const ImageMetrics&) -> std::ostream&;

} // namespace sieve

GCLIB_REGISTER_ENUM_TYPE(sieve::ImageMetric, 1);
