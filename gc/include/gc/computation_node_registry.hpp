/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/computation_node_fwd.hpp"
#include "gc/node_registry.hpp"


namespace gc {

using ComputationNodeRegistry = NodeRegistry<ComputationNode>;

inline auto computation_node_registry()
    -> ComputationNodeRegistry
{ return node_registry<ComputationNode>(); }

} // namespace gc
