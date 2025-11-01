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

#include "gc_app/types/uint.hpp"

#include "gc/value.hpp"


namespace gc_app {

using UintVec = std::vector<Uint>;

inline auto uint_vec_val(const UintVec& value)
    -> gc::Value
{ return value; }

inline auto uint_vec_val(UintVec&& value)
    -> gc::Value
{ return std::move(value); }

inline auto uint_vec_val(gc::Value& value)
    -> const UintVec&
{ return value.as<UintVec>(); }

inline auto uint_vec_val(const gc::Value& value)
    -> const UintVec&
{ return value.as<UintVec>(); }

} // namespace gc_app
