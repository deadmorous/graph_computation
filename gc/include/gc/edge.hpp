#pragma once

#include "gc/node_index.hpp"
#include "gc/port.hpp"

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
using EdgeOutputEnd = EdgeEnd<Output_Tag>;

struct Edge
{
    EdgeOutputEnd from;
    EdgeInputEnd to;

    auto operator<=>(const Edge&) const noexcept
        -> std::strong_ordering = default;
};

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
