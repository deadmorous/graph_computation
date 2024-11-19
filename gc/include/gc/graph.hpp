#pragma once

#include "gc/edge.hpp"
#include "gc/node_fwd.hpp"

#include <vector>


namespace gc {

struct Graph final
{
    std::vector<NodePtr>    nodes;
    std::vector<Edge>       edges;
};

} // namespace gc
