#include "gc/yaml/parse_graph.hpp"

#include "gc/detail/parse_node_port.hpp"
#include "gc/value.hpp"
#include "gc/yaml/parse_value.hpp"

#include "common/throw.hpp"

#include "yaml-cpp/yaml.h"

namespace gc::yaml {

auto parse_graph(const YAML::Node& config,
                const NodeRegistry& node_registry,
                const TypeRegistry& type_registry)
    -> std::pair< Graph, detail::NamedNodes >
{
    auto g = Graph{};
    auto node_map = std::unordered_map<std::string, const Node*>{};
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

        g.nodes.push_back(graph_node);
        node_map.emplace(name, graph_node.get());
    }

    // Parse graph edges
    for (auto edge : config["edges"])
    {
        auto e0 = detail::parse_node_port(
            edge[0].as<std::string>(), node_map, Output);
        auto e1 = detail::parse_node_port(
            edge[1].as<std::string>(), node_map, Input);
        g.edges.push_back({e0, e1});
    }

    return { g, node_map };
}

} // namespace gc::yaml
