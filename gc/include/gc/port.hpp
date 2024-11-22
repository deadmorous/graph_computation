#pragma once

#include "gc/node_port_tags.hpp"

#include "common/strong.hpp"

#include <cstdint>


namespace gc {

GCLIB_STRONG_TYPE(InputPortCount, uint8_t, 0, common::StrongCountFeatures);

GCLIB_STRONG_TYPE(OutputPortCount, uint8_t, 0, common::StrongCountFeatures);

GCLIB_STRONG_TYPE(
    InputPort, uint8_t, 0, common::StrongIndexFeatures<InputPortCount>);

GCLIB_STRONG_TYPE(
    OutputPort, uint8_t, 0, common::StrongIndexFeatures<OutputPortCount>);

namespace literals {
GCLIB_STRONG_LITERAL_SUFFIX(InputPortCount, _gc_ic);
GCLIB_STRONG_LITERAL_SUFFIX(InputPort, _gc_i);
GCLIB_STRONG_LITERAL_SUFFIX(OutputPortCount, _gc_oc);
GCLIB_STRONG_LITERAL_SUFFIX(OutputPort, _gc_o);
} // namespace literals

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
