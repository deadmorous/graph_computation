#pragma once

#include "agc/node_fwd.hpp"

#include "gc/value_fwd.hpp"

namespace agc_app {

auto make_linspace(gc::ConstValueSpan)
    -> std::shared_ptr<agc::Node>;

} // namespace agc_app
