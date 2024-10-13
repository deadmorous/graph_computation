#include "gc/yaml/parse_value.hpp"

#include "gc/type.hpp"
#include "gc/value.hpp"

#include "common/throw.hpp"

#include "yaml-cpp/yaml.h"

#include <charconv>

namespace gc::yaml {

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

} // anonymous namespace

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

} // namespace gc::yaml
