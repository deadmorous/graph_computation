#pragma once

#include "gc/node_port_tags.hpp"

#include "common/strong.hpp"

#include <cstdint>


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


GCLIB_STRONG_TYPE(InputPortCount, uint8_t);
GCLIB_STRONG_TYPE(OutputPortCount, uint8_t);

constexpr inline auto operator<=>(const InputPort& a,
                                  const InputPortCount& b) noexcept
    -> std::strong_ordering
{ return a.v <=> b.v; }

constexpr inline auto operator<=>(const InputPortCount& a,
                                  const InputPort& b) noexcept
    -> std::strong_ordering
{ return a.v <=> b.v; }

constexpr inline auto operator<=>(const OutputPort& a,
                                  const OutputPortCount& b) noexcept
    -> std::strong_ordering
{ return a.v <=> b.v; }

constexpr inline auto operator<=>(const OutputPortCount& a,
                                  const OutputPort& b) noexcept
    -> std::strong_ordering
{ return a.v <=> b.v; }

} // namespace gc
