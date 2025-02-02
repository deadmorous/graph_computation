#pragma once

#include "gc/edge.hpp"

#include "common/strong_vector.hpp"


namespace gc {

template <typename Node_, typename Edge_=Edge>
struct Graph final
{
    using Node = Node_;
    using Edge = Edge_;
    using NodeVec = common::StrongVector<Node, NodeIndex>;
    using EdgeVec = std::vector<Edge>;

    NodeVec nodes;
    EdgeVec edges;
};

} // namespace gc
