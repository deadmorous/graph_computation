#include "gc/activation_node_fwd.hpp"
#include "gc/graph.hpp"

namespace gc {

using ActivationGraph = Graph<ActivationNode>;

auto generate_source(const ActivationGraph& g)
    -> void;

} // namespace gc
