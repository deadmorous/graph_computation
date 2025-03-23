/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/activation_graph.hpp"
#include "gc/node_labels.hpp"

#include <ostream>


namespace gc {

auto generate_dot(std::ostream& s,
                  const gc::ActivationGraph& g,
                  gc::NodeLabels node_labels)
    -> void;

} // namespace gc
