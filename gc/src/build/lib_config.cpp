/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "build/lib_config.hpp"

#include "mpk/mix/value/value.hpp"


namespace build {

auto operator<<(std::ostream& s, const LibConfig& lib_config)
    -> std::ostream&
{ return s << mpk::mix::value::Value{ lib_config }; }

} // namespace build
