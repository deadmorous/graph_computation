/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "sieve/types/i8_range.hpp"
#include "sieve/types/image_metrics.hpp"

#include "gc_app/types/image.hpp"

namespace sieve {

auto image_metrics(const gc_app::I8Image& img,
                   I8Range state_range,
                   ImageMetricSet metric_types)
    -> ImageMetrics;

} // namespace sieve
