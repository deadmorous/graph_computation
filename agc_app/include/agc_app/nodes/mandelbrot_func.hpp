#pragma once

#include "gc/activation_node_fwd.hpp"
#include "gc/alg_type_literal.hpp"
#include "gc/decl_literal.hpp"
#include "gc/value_fwd.hpp"

namespace agc_app {

auto make_mandelbrot_func(gc::ConstValueSpan)
    -> std::shared_ptr<gc::ActivationNode>;

GCLIB_DECL_LITERAL(gc::alg::TypeLiteral, mandelbrot_point_type);

} // namespace agc_app
