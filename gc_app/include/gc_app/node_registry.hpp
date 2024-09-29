#pragma once

#include "gc/graph_computation.hpp"

#include "common/object_registry.hpp"

namespace gc_app {

using NodeRegistry =
    common::ObjectRegsstry<gc::Node>;

auto node_registry()
    -> NodeRegistry;

} // namespace gc_app
