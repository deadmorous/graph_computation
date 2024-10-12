#pragma once

#include "gc/node_registry.hpp"

namespace gc_app {

auto populate_node_registry(gc::NodeRegistry&)
    -> void;

inline auto node_registry()
    -> gc::NodeRegistry
{
    auto result = gc::NodeRegistry{};
    populate_node_registry(result);
    return result;
}

} // namespace gc_app
