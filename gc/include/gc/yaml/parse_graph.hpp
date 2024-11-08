#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/graph.hpp"
#include "gc/source_inputs.hpp"
#include "gc/node_registry.hpp"
#include "gc/type_registry.hpp"
#include "gc/value_fwd.hpp"

#include "yaml-cpp/node/node.h"


namespace gc::yaml {

struct ParseGraphResult final
{
    Graph                       graph;
    SourceInputs                inputs;
    detail::NamedNodes          node_names;
    std::vector<std::string>    input_names;
};

auto parse_graph(const YAML::Node&, const NodeRegistry&, const TypeRegistry&)
    -> ParseGraphResult;

} // namespace gc::yaml
