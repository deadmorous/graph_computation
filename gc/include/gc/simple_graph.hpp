/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/graph.hpp"
#include "gc/simple_edge.hpp"


namespace gc {

template <typename Node>
using SimpleGraph = Graph<Node, SimpleEdge<Node>>;

} // namespace gc
