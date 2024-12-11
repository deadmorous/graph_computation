#pragma once

#include "gc/activation_node_fwd.hpp"

#include "gc/value_fwd.hpp"

namespace agc_app {

auto make_linspace(gc::ConstValueSpan)
    -> std::shared_ptr<gc::ActivationNode>;

} // namespace agc_app