#pragma once

#include "gc/graph.hpp"

#include "gc/computation_node_fwd.hpp"

#include <memory>


namespace gc {

using ComputationGraph = Graph<std::shared_ptr<ComputationNode>>;

} // namespace gc
