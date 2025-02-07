#pragma once

#include "gc/node_registry.hpp"


namespace agc_app {

template <typename Node>
auto populate_node_registry(gc::NodeRegistry<Node>&)
    -> void;

template <typename Node>
auto node_registry(common::Type_Tag<Node> = {})
    -> gc::NodeRegistry<Node>
{
    auto result = gc::NodeRegistry<Node>{};
    populate_node_registry(result);
    return result;
}

} // namespace agc_app
