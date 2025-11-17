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

#include "gc/computation_node_fwd.hpp"
#include "gc/computation_context_fwd.hpp"
#include "gc/value_fwd.hpp"

namespace gc_app::cell_aut {

auto make_generate_cmap(gc::ConstValueSpan, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>;

} // namespace gc_app::cell_aut
