#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_fwd.hpp"

#include "common/object_registry.hpp"

namespace gc {

using NodeRegistry =
    common::ObjectRegstry<Node, ConstValueSpan>;

auto populate_gc_node_registry(NodeRegistry&)
    -> void;

inline auto node_registry()
    -> NodeRegistry
{
    auto result = NodeRegistry{};
    populate_gc_node_registry(result);
    return result;
}

} // namespace gc
