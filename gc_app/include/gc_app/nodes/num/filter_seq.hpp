/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/computation_node_fwd.hpp"
#include "gc/computation_context_fwd.hpp"
#include "mpk/mix/value/value_fwd.hpp"

namespace gc_app::num {

auto make_filter_seq(mpk::mix::value::ConstValueSpan, const gc::ComputationContext&)
-> std::shared_ptr<gc::ComputationNode>;

} // namespace gc_app::num
