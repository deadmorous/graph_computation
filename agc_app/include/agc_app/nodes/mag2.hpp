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
#include "gc/alg_type_literal.hpp"
#include "gc/decl_literal.hpp"
#include "gc/value_fwd.hpp"

namespace agc_app {

auto make_mag2(gc::ConstValueSpan, const gc::ActivationContext&)
    -> std::shared_ptr<gc::ActivationNode>;

GCLIB_DECL_LITERAL(gc::alg::TypeLiteral, mag2_input_type);
GCLIB_DECL_LITERAL(gc::alg::TypeLiteral, mag2_output_type);

} // namespace agc_app
