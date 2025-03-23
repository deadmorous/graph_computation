/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/type_fwd.hpp"
#include "gc/value_fwd.hpp"

#include <string_view>


namespace gc {

auto parse_simple_value(std::string_view, const gc::Type*)
    -> gc::Value;

} // namespace gc
