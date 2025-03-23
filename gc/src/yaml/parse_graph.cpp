/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/yaml/parse_graph.hpp"

#include "gc/computation_node_registry.hpp"
#include "gc/detail/parse_node_port.hpp"
#include "gc/value.hpp"
#include "gc/yaml/parse_value.hpp"

#include "common/throw.hpp"

#include <yaml-cpp/yaml.h>

namespace gc::yaml {

template <typename Node>
auto parse_graph(const YAML::Node& config,
                 const NodeRegistry<Node>& node_registry,
                 const TypeRegistry& type_registry)
    -> ParseGraphResult<Node>
{
    auto g = ComputationGraph{};
    auto node_map = detail::NamedNodes<Node>{};
    auto node_indices = detail::NodeIndices<Node>{};
    for (auto node : config["nodes"])
    {
        auto name = node["name"].as<std::string>();
        auto type = node["type"].as<std::string>();

        auto init = gc::ValueVec{};
        if (auto init_ = node["init"])
            for (auto element_ : init_)
            {
                auto element = parse_value(element_, type_registry);
                init.push_back(element);
            }

        auto graph_node =
            node_registry.at(type)(init);

        node_indices.emplace(graph_node.get(), NodeIndex{} + g.nodes.size());
        node_map.emplace(name, graph_node.get());
        g.nodes.push_back(graph_node);
    }

    // Parse graph edges
    for (auto edge : config["edges"])
    {
        auto e0 = detail::parse_node_port(
            edge[0].as<std::string>(), node_map, node_indices, Output);
        auto e1 = detail::parse_node_port(
            edge[1].as<std::string>(), node_map, node_indices, Input);
        g.edges.push_back({e0, e1});
    }

    // Parse graph source inputs
    auto source_inputs = gc::SourceInputs{};
    auto input_names = std::vector<std::string>{};
    for (auto in : config["inputs"])
    {
        input_names.push_back(in["name"].as<std::string>());
        source_inputs.values.push_back(parse_value(in, type_registry));
        for (auto d_ : in["destinations"])
        {
            auto d = detail::parse_node_port(
                d_.as<std::string>(), node_map, node_indices, Input);
            add_to_last_group(source_inputs.destinations, d);
        }
        next_group(source_inputs.destinations);
    }

    return {
        .graph = std::move(g),
        .inputs = source_inputs,
        .node_names = std::move(node_map),
        .input_names = std::move(input_names) };
}

template
auto parse_graph<ComputationNode>(
                const YAML::Node& config,
                const ComputationNodeRegistry& node_registry,
                const TypeRegistry& type_registry)
    -> ParseComputationGraphResult;

} // namespace gc::yaml
