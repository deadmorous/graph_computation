#pragma once

#include "gc_app/node_registry.hpp"

#include "gc/computation_node_registry.hpp"


namespace gc_app {

inline auto computation_node_registry()
    -> gc::ComputationNodeRegistry
{ return node_registry<gc::ComputationNode>(); }

} // namespace gc_app
