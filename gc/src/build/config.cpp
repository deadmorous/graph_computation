/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "build/config.hpp"

#include "gc/value.hpp"


namespace build {

auto operator<<(std::ostream& s, const Config& config)
    -> std::ostream&
{ return s << gc::Value{ config }; }

} // namespace build
