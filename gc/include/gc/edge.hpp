#pragma once

#include "gc/node_port_tags.hpp"

#include "common/strong.hpp"

#include <compare>
#include <cstdint>
#include <ostream>


namespace gc {

GCLIB_STRONG_TYPE(InputPort, uint8_t);
GCLIB_STRONG_TYPE(OutputPort, uint8_t);

template <PortTagType Tag>
struct PortType;

template <PortTagType Tag>
using PortType_t = PortType<Tag>::type;

template <PortTagType Tag>
using Port = PortType_t<Tag>;

template <>
struct PortType<Input_Tag> final
{ using type = InputPort; };

template <>
struct PortType<Output_Tag> final
{ using type = OutputPort; };

template <PortTagType Tag>
struct EdgeEnd final
{
    uint32_t node;
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
