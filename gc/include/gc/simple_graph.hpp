#pragma once

#include "gc/graph.hpp"
#include "gc/simple_edge.hpp"


namespace gc {

template <typename Node>
using SimpleGraph = Graph<Node, SimpleEdge<Node>>;

} // namespace gc
