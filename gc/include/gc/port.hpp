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

#include "gc/node_port_tags.hpp"
#include "gc/weak_port.hpp"

#include "common/strong.hpp"

#include <ostream>


namespace gc {

GCLIB_STRONG_TYPE(InputPortCount, WeakPort, common::StrongCountFeatures);

GCLIB_STRONG_TYPE(OutputPortCount, WeakPort, common::StrongCountFeatures);

GCLIB_STRONG_TYPE(
    InputPort, WeakPort, common::StrongIndexFeatures<InputPortCount>);

GCLIB_STRONG_TYPE(
    OutputPort, WeakPort, common::StrongIndexFeatures<OutputPortCount>);

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

inline auto operator<<(std::ostream& s, InputPort p)
    -> std::ostream&
{ return s << "in_" << static_cast<uint32_t>(p.v); }

inline auto operator<<(std::ostream& s, OutputPort p)
    -> std::ostream&
{ return s << "out_" << static_cast<uint32_t>(p.v); }

} // namespace gc
