/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/node_index.hpp"
#include "gc/port.hpp"
#include "common/struct_type_macro.hpp"

#include <compare>


namespace gc {

template <PortTagType Tag>
struct EdgeEnd final
{
    NodeIndex node;
    Port<Tag> port;

    auto operator<=>(const EdgeEnd&) const noexcept
        -> std::strong_ordering = default;
};

using EdgeInputEnd = EdgeEnd<Input_Tag>;
GCLIB_STRUCT_TYPE(EdgeInputEnd, node, port);

using EdgeOutputEnd = EdgeEnd<Output_Tag>;
GCLIB_STRUCT_TYPE(EdgeOutputEnd, node, port);

struct Edge
{
    EdgeOutputEnd from;
    EdgeInputEnd to;

    auto operator<=>(const Edge&) const noexcept
        -> std::strong_ordering = default;
};
GCLIB_STRUCT_TYPE(Edge, from, to);

inline auto edge(EdgeOutputEnd from, EdgeInputEnd to)
    -> Edge
{ return { from, to }; }

auto operator<<(std::ostream& s, const EdgeInputEnd& ee)
    -> std::ostream&;

auto operator<<(std::ostream& s, const EdgeOutputEnd& ee)
    -> std::ostream&;

auto operator<<(std::ostream& s, const Edge& e)
    -> std::ostream&;

} // namespace gc
