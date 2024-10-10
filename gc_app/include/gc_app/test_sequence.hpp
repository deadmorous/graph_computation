#pragma once

#include "gc/node_fwd.hpp"

namespace gc_app {

auto make_test_sequence()
    -> std::shared_ptr<gc::Node>;

} // namespace gc_app
