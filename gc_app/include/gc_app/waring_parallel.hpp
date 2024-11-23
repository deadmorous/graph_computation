#pragma once

#include "gc/computation_node_fwd.hpp"
#include "gc/value_fwd.hpp"

namespace gc_app {

auto make_waring_parallel(gc::ConstValueSpan)
    -> std::shared_ptr<gc::ComputationNode>;

} // namespace gc_app
