#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_fwd.hpp"

namespace gc_app {

auto make_filter_seq(gc::ConstValueSpan)
-> std::shared_ptr<gc::Node>;

} // namespace gc_app
