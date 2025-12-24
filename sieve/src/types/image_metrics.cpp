/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "sieve/types/image_metrics.hpp"

#include "gc/value.hpp"

namespace sieve {

auto operator<<(std::ostream& s, const ImageMetrics& m) -> std::ostream&
{
    return s << gc::Value{m};
}

} // namespace sieve
