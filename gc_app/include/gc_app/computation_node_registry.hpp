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

#include "gc_app/node_registry.hpp"

#include "gc/computation_node_registry.hpp"


namespace gc_app {

inline auto computation_node_registry()
    -> gc::ComputationNodeRegistry
{ return node_registry<gc::ComputationNode>(); }

} // namespace gc_app
