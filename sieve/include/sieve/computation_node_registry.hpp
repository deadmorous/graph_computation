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

#include "sieve/node_registry.hpp"

#include "gc/computation_node_registry.hpp"


namespace sieve {

inline auto computation_node_registry()
    -> gc::ComputationNodeRegistry
{ return node_registry<gc::ComputationNode>(); }

} // namespace sieve
