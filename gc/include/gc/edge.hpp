#pragma once

#include "gc/node_fwd.hpp"

#include <array>
#include <compare>
#include <cstdint>
#include <ostream>


namespace gc {

struct EdgeEnd final
{
    uint32_t node;
    uint32_t port;

    auto operator<=>(const EdgeEnd&) const noexcept
        -> std::strong_ordering = default;
};

using Edge = std::array<EdgeEnd, 2>;

inline auto edge(EdgeEnd from, EdgeEnd to)
    -> Edge
{ return { from, to }; }

auto operator<<(std::ostream& s, const EdgeEnd& ee)
    -> std::ostream&;

auto operator<<(std::ostream& s, const Edge& e)
    -> std::ostream&;

} // namespace gc
