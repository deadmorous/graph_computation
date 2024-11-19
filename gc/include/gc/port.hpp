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

} // namespace gc
