#pragma once

#include "gc/graph_computation.hpp"

namespace gc_app {

auto make_rect_view()
    -> std::shared_ptr<gc::Node>;

} // namespace gc_app