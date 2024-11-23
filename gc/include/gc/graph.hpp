#pragma once

#include "gc/edge.hpp"

#include "common/strong_vector.hpp"

#include <memory>


namespace gc {

template <typename Node_>
struct Graph final
{
    using Node = Node_;
    using NodePtr = std::shared_ptr<Node>;
    using NodeVec = common::StrongVector<NodePtr, NodeIndex>;
    using EdgeVec = std::vector<Edge>;

    NodeVec nodes;
    EdgeVec edges;
};

} // namespace gc
