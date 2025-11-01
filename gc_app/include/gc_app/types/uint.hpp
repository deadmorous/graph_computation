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

#include "gc/value.hpp"


namespace gc_app {

using Uint = uint32_t;

inline auto uint_val(Uint value)
    -> gc::Value
{ return value; }

inline auto uint_val(gc::Value& value)
    -> Uint&
{ return value.as<Uint>(); }

inline auto uint_val(const gc::Value& value)
    -> const Uint&
{ return value.as<Uint>(); }

} // namespace gc_app
