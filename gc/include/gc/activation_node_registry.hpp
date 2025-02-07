#pragma once

#include "gc/activation_node_fwd.hpp"
#include "gc/node_registry.hpp"


namespace gc {

using ActivationNodeRegistry = NodeRegistry<ActivationNode>;

inline auto activation_node_registry()
    -> ActivationNodeRegistry
{ return node_registry<ActivationNode>(); }

} // namespace gc
