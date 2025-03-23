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

#include "agc_app/node_registry.hpp"

#include "gc/activation_node_registry.hpp"


namespace agc_app {

inline auto activation_node_registry()
    -> gc::ActivationNodeRegistry
{ return node_registry<gc::ActivationNode>(); }

} // namespace gc_app
