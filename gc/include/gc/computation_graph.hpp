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

#include "gc/graph.hpp"

#include "gc/computation_node_fwd.hpp"

#include <memory>


namespace gc {

using ComputationGraph = Graph<std::shared_ptr<ComputationNode>>;

} // namespace gc
