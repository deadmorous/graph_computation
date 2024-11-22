#pragma once

#include "gc/edge.hpp"
#include "gc/node_fwd.hpp"

#include "common/strong_vector.hpp"

#include <vector>


namespace gc {

struct Graph final
{
    using NodeVec = common::StrongVector<NodePtr, NodeIndex>;
    using EdgeVec = std::vector<Edge>;

    NodeVec nodes;
    EdgeVec edges;
};

} // namespace gc
