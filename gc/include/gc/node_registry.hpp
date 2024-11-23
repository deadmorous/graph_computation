#pragma once

#include "gc/value_fwd.hpp"

#include "common/object_registry.hpp"
#include "common/type.hpp"


namespace gc {

template <typename Node>
using NodeRegistry =
    common::ObjectRegistry<Node, ConstValueSpan>;

template <typename Node>
auto populate_gc_node_registry(NodeRegistry<Node>&)
    -> void;

template <typename Node>
auto node_registry(common::Type_Tag<Node> = {})
    -> NodeRegistry<Node>
{
    auto result = NodeRegistry<Node>{};
    populate_gc_node_registry(result);
    return result;
}

} // namespace gc
