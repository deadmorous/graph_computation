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

#include "gc/activation_context_fwd.hpp"
#include "gc/activation_node_fwd.hpp"

#include "mpk/mix/value/value_fwd.hpp"

namespace agc_app {

auto make_threshold(mpk::mix::value::ConstValueSpan, const gc::ActivationContext&)
    -> std::shared_ptr<gc::ActivationNode>;

} // namespace agc_app
