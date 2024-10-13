#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/graph.hpp"
#include "gc/node_registry.hpp"
#include "gc/type_registry.hpp"
#include "gc/value_fwd.hpp"

#include "yaml-cpp/node/node.h"


namespace gc::yaml {

auto parse_graph(const YAML::Node&, const NodeRegistry&, const TypeRegistry&)
    -> std::pair< Graph, detail::NamedNodes >;

} // namespace gc::yaml
