#pragma once

#include "gc/graph.hpp"
#include "gc/node_registry.hpp"
#include "gc/type_registry.hpp"

#include "yaml-cpp/node/node.h"

#include <string>
#include <unordered_map>

namespace gc {

using NamedNodes =
    std::unordered_map<std::string, const Node*>;

auto load_graph(const YAML::Node&, const NodeRegistry&, const TypeRegistry&)
    -> std::pair< Graph, NamedNodes >;

} // namespace gc
