#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/computation_graph.hpp"
#include "gc/source_inputs.hpp"
#include "gc/node_registry.hpp"
#include "gc/type_registry.hpp"
#include "gc/value_fwd.hpp"

#include <yaml-cpp/node/node.h>


namespace gc::yaml {

template <typename Node>
struct ParseGraphResult final
{
    ComputationGraph            graph;
    SourceInputs                inputs;
    detail::NamedNodes<Node>    node_names;
    std::vector<std::string>    input_names;
};

using ParseComputationGraphResult = ParseGraphResult<ComputationNode>;

template <typename Node>
auto parse_graph(const YAML::Node&,
                 const NodeRegistry<Node>&, const TypeRegistry&)
    -> ParseGraphResult<Node>;

} // namespace gc::yaml
