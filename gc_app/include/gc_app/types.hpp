#pragma once

#include "gc/value.hpp"


namespace gc_app {

using Uint = uint32_t;
using UintVec = std::vector<Uint>;



inline auto uint_val(Uint value)
    -> gc::Value
{ return value; }

inline auto uint_val(gc::Value& value)
    -> Uint&
{ return value.as<Uint>(); }

inline auto uint_val(const gc::Value& value)
    -> const Uint&
{ return value.as<Uint>(); }



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
