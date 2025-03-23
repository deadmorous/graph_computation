/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

// #include "gc/node_index.hpp"
// #include "common/struct_type_macro.hpp"

#include <compare>
#include <iostream>
#include <type_traits>


namespace gc {

template <typename T>
struct SimpleEdge
{
    T from;
    T to;

    auto operator<=>(const SimpleEdge&) const noexcept
        -> std::strong_ordering = default;
};

template <typename T>
inline auto simple_edge(T from, std::type_identity_t<T> to)
    -> SimpleEdge< std::remove_cvref_t<T> >
{ return { from, to }; }

template <typename T>
inline auto operator<<(std::ostream& s, const SimpleEdge<T>& e)
    -> std::ostream&
{ return s << '[' << e.from << "->" << e.to << ']'; }

} // namespace gc
