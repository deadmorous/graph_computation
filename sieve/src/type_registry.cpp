/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "sieve/type_registry.hpp"
#include "sieve/types/image_metrics.hpp"

#include "mpk/mix/value/type.hpp"

namespace sieve {

auto populate_type_registry(gc::TypeRegistry& result)
    -> void
{
    result.register_value("ImageMetric", mpk::mix::value::type_of<ImageMetric>());
    result.register_value("ImageMetricSet", mpk::mix::value::type_of<ImageMetricSet>());
    result.register_value("ImageMetrics", mpk::mix::value::type_of<ImageMetrics>());
}

} // namespace sieve
