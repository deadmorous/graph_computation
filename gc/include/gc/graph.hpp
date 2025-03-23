/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

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
