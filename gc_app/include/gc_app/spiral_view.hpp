#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_fwd.hpp"

namespace gc_app {

auto make_spiral_view(gc::ConstValueSpan)
    -> std::shared_ptr<gc::Node>;

} // namespace gc_app
