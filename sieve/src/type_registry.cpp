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

#include "gc/type.hpp"

namespace sieve {

auto populate_type_registry(gc::TypeRegistry& result)
    -> void
{
    result.register_value("ImageMetrics", gc::type_of<ImageMetrics>());
}

} // namespace sieve
