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

#include "gc/activation_node_fwd.hpp"

#include "gc/value_fwd.hpp"

namespace agc_app {

auto make_replicate(gc::ConstValueSpan)
    -> std::shared_ptr<gc::ActivationNode>;

} // namespace agc_app
