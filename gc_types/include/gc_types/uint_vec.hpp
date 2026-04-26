/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_types/uint.hpp"

#include "mpk/mix/value/value.hpp"


namespace gc_types {

using UintVec = std::vector<Uint>;

inline auto uint_vec_val(const UintVec& value)
    -> mpk::mix::value::Value
{ return value; }

inline auto uint_vec_val(UintVec&& value)
    -> mpk::mix::value::Value
{ return std::move(value); }

inline auto uint_vec_val(mpk::mix::value::Value& value)
    -> const UintVec&
{ return value.as<UintVec>(); }

inline auto uint_vec_val(const mpk::mix::value::Value& value)
    -> const UintVec&
{ return value.as<UintVec>(); }

} // namespace gc_types
