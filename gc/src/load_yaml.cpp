#include "gc/load_yaml.hpp"

#include "gc/node.hpp"

#include "common/throw.hpp"

#include "yaml-cpp/yaml.h"

#include <charconv>


namespace gc {

namespace {

struct ScalarParser
{
    template <typename T>
    requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
    auto operator()(common::Type_Tag<T>, Value& v, std::string_view s) const
        -> void
    {
        auto typed = T{};
        auto fcres = std::from_chars(s.begin(), s.end(), typed);

        if (fcres.ec == std::errc::invalid_argument)
            common::throw_(
                "Failed to parse scalar of type ", type_of<T>(),
                " from string '", s, "' - not a number");
        else if (fcres.ec == std::errc::result_out_of_range)
            common::throw_(
                "Failed to parse scalar of type ", type_of<T>(),
                " from string '", s, "' - out of range");

        assert (fcres.ec == std::error_code{});
        if(fcres.ptr != s.end())
            common::throw_(
                "Failed to parse scalar of type ", type_of<T>(),
                " from string '", s, "' - extra characters remain");

        v = typed;
    }

    auto operator()(common::Type_Tag<bool>, Value& v, std::string_view s) const
        -> void
    {
        if (s == "true")
            v = true;
        else if (s == "false")
            v = false;
        else
            common::throw_(
                "Failed to parse boolean scalar from string '", s, "'");
    }

    auto operator()(common::Type_Tag<std::byte>, Value&, std::string_view) const
        -> void
    {
        common::throw_("TODO: Parse std::byte");
    }
};

struct YamlValueParser
{
    auto operator()(const CustomT& t, Value& value, const YAML::Node& node)
        -> void
    {
        common::throw_(
            "YamlValueParser: Failed to parse value of type ", t.type(),
            " because custom types are not supported");
    }
    auto operator()(const PathT& t, Value& value, const YAML::Node& node)
        -> void
    {
        value = ValuePath::from_string(node.as<std::string>());
    }
    auto operator()(const ScalarT& t, Value& value, const YAML::Node& node)
        -> void
    {
        t.visit(ScalarParser{}, value, node.as<std::string>());
    }

    auto operator()(const StringT& t, Value& value, const YAML::Node& node)
        -> void
    {
        value = node.as<std::string>();
    }

    auto operator()(const StructT& t, Value& value, const YAML::Node& node)
        -> void
    {
        for (const auto& key: value.keys())
        {
            auto field_value_node = node[key.name()];
            if (!field_value_node)
                continue;

            auto path = ValuePath{} / key;

            auto field_value = value.get(path);

            visit(field_value.type(),
                  YamlValueParser{},
                  field_value,
                  field_value_node);

            value.set(path, field_value);
        }
    }

    auto operator()(const TupleT& t, Value& value, const YAML::Node& node)
        -> void
    {
        auto keys = value.keys();
        if (node.size() != keys.size())
            common::throw_(
                "YamlValueParser: Failed to parse value of type ", t.type(),
                " because of tuple size mismatch");

        for (const auto& key: keys)
        {
            auto element_value_node = node[key.index()];

            auto path = ValuePath{} / key;
            auto element_value = value.get(path);
            visit(element_value.type(),
                  YamlValueParser{},
                  element_value,
                  element_value_node);

            value.set(path, element_value);
        }
    }

    auto operator()(const VectorT& t, Value& value, const YAML::Node& node)
        -> void
    {
        value.resize({}, node.size());
        for (const auto& key: value.keys())
        {
            auto element_value_node = node[key.index()];

            auto path = ValuePath{} / key;
            auto element_value = value.get(path);
            visit(element_value.type(),
                  YamlValueParser{},
                  element_value,
                  element_value_node);

            value.set(path, element_value);
        }
    }
};

auto parse_value(const YAML::Node& node,
                 const TypeRegistry& type_registry)
    -> Value
{
    auto type_name = node["type"].as<std::string>();
    const auto* type = type_registry.at(type_name);

    auto value_node = node["value"];
    if (!value_node)
        common::throw_("parse_value failed: Value is missing for node\n", node);

    auto result = Value::make(type);
    visit(type, YamlValueParser{}, result, value_node);

    return result;
}

} // anonymous namespace

auto load_graph(const YAML::Node& config,
                const NodeRegistry& node_registry,
                const TypeRegistry& type_registry)
    -> std::pair< Graph, NamedNodes >
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

    auto input_ports = [](const Node* node)
    { return node->input_names(); };

    auto output_ports = [](const Node* node)
    { return node->output_names(); };

    auto edge_end_from_str = [&](const std::string& ee_str, auto port_names)
        -> EdgeEnd
    {
        // Parse node name and optional port name/index
        std::string node_name;
        std::string_view port_name;
        if (auto pos = ee_str.find_last_of('.'); pos != std::string::npos)
        {
            node_name = ee_str.substr(0, pos);
            port_name = { ee_str.data() + pos + 1, ee_str.size() - pos - 1 };
        }
        else
            node_name = ee_str;

        // Find graph node referenced by edge end
        auto node_it = node_map.find(node_name);
        if (node_it == node_map.end())
            common::throw_("Node '", node_name, "' is not found");
        const auto* node = node_it->second;

        // Get node port names
        auto pnames = port_names(node);
        if (pnames.empty())
            common::throw_(
                "Edge end '", ee_str, "' is invalid because node has no ports");

        // Resolve port index
        auto port = uint32_t{0};
        if (port_name.empty())
        {
            // Default index 0 can only be used if there is exactly one port
            if (pnames.size() != 1)
                common::throw_(
                    "Port name must be specified for node ", node_name,
                    ": please specify one of ", common::format_seq(pnames));
        }
        else if (
            // Try parsing as number, treat `port_name` as a name if that fails
            auto fcres =
                std::from_chars(port_name.begin(), port_name.end(), port);
            fcres.ec != std::error_code{} || fcres.ptr != port_name.end())
        {
            // Parsing as number failed, find port index
            auto it = std::find(pnames.begin(), pnames.end(), port_name);
            if (it == pnames.end())
                // No such port
                common::throw_(
                    "Node ", node_name, " does not have port ", port_name,
                    ": please specify one of ", common::format_seq(pnames));
            port = it - pnames.begin();
        }
        else if (port >= pnames.size())
            // Check port index bounds
            common::throw_(
                "Number of ports in node ", node_name, " is ", pnames.size(),
                ", port index ", port, " is out of range");
        return {node, port};
    };

    // Parse graph edges
    for (auto edge : config["edges"])
    {
        auto e0 = edge_end_from_str(edge[0].as<std::string>(), output_ports);
        auto e1 = edge_end_from_str(edge[1].as<std::string>(), input_ports);
        g.edges.push_back({e0, e1});
    }

    return { g, node_map };
}

} // namespace gc
