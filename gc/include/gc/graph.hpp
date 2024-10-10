#pragma once

#include "gc/edge.hpp"

#include <vector>


namespace gc {

struct Graph final
{
    std::vector<NodePtr>    nodes;
    std::vector<Edge>       edges;
};

} // namespace gc
