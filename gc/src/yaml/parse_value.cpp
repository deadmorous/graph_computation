/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/yaml/parse_value.hpp"

#include "gc/parse_simple_value.hpp"
#include "gc/type.hpp"
#include "gc/value.hpp"

#include "common/throw.hpp"

#include <yaml-cpp/yaml.h>

#include <charconv>

using namespace std::string_view_literals;

namespace gc::yaml {

namespace {

struct YamlValueParser final
{
    auto operator()(const ArrayT& t, Value& value, const YAML::Node& node) const
        -> void
    {
        if (value.size() != node.size())
            common::throw_(
                "YamlValueParser: Failed to parse value of type ", t.type(),
                " because actual array size ", node.size(), " differs from "
                "expected size ", value.size());

        for (const auto& key: value.path_item_keys())
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

    auto operator()(const CustomT& t, Value& value, const YAML::Node& node) const
        -> void
    {
        common::throw_(
            "YamlValueParser: Failed to parse value of type ", t.type(),
            " because custom types are not supported");
    }
    auto operator()(const EnumT& t, Value& value, const YAML::Node& node) const
        -> void
    { value = parse_simple_value(node.as<std::string>(), t.type()); }

    auto operator()(const PathT& t, Value& value, const YAML::Node& node) const
        -> void
    { value = parse_simple_value(node.as<std::string>(), t.type()); }

    auto operator()(const ScalarT& t, Value& value, const YAML::Node& node) const
        -> void
    { value = parse_simple_value(node.as<std::string>(), t.type()); }

    auto operator()(const SetT& t, Value& value, const YAML::Node& node) const
        -> void
    {
        value.set_default();
        const auto* key_type = t.key_type();
        if (node.IsSequence())
        {
            for (size_t index=0, n=node.size(); index<n; ++index)
            {
                auto key_node = node[index];
                auto k = Value::make(key_type);
                visit(key_type, YamlValueParser{}, k, key_node);
                value.insert(k);
            }
        }
        else if (node.IsScalar())
        {
            auto node_str = node.as<std::string>();
            if (node_str == "all")
            {
                auto k = Value::make(key_type);
                auto names =
                    k.get(ValuePath{"names"sv})
                        .as<std::vector<std::string_view>>();
                for (auto name : names)
                {
                    k.set(ValuePath{"name"sv}, std::string{name});
                    value.insert(k);
                }
            }
            else
                common::throw_(
                    "YamlValueParser: Failed to parse value of type ", t.type(),
                    " - if specified as a scalar, only 'all' is recognized");

        }
        else
            common::throw_(
                "YamlValueParser: Failed to parse value of type ", t.type(),
                " - node is neither an array nor a scalar");
    }

    auto operator()(const StringT& t, Value& value, const YAML::Node& node) const
        -> void
    { value = parse_simple_value(node.as<std::string>(), t.type()); }

    auto operator()(const StrongT& t, Value& value, const YAML::Node& node) const
        -> void
    { value = parse_simple_value(node.as<std::string>(), t.type()); }

    auto operator()(const StructT& t, Value& value, const YAML::Node& node) const
        -> void
    {
        for (const auto& key: value.path_item_keys())
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

    auto operator()(const TupleT& t, Value& value, const YAML::Node& node) const
        -> void
    {
        auto keys = value.path_item_keys();
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

    auto operator()(const VectorT& t, Value& value, const YAML::Node& node) const
        -> void
    {
        value.resize({}, node.size());
        for (const auto& key: value.path_item_keys())
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
                 const Type* type,
                 const TypeRegistry& type_registry)
    -> Value
{
    auto result = Value::make(type);
    visit(type, YamlValueParser{}, result, node);

    return result;
}

auto parse_value(const YAML::Node& node,
                 const TypeRegistry& type_registry)
    -> Value
{
    auto type_name = node["type"].as<std::string>();
    const auto* type = type_registry.at(type_name);

    auto value_node = node["value"];
    if (!value_node)
        common::throw_("parse_value failed: Value is missing for node\n", node);

    return parse_value(value_node, type, type_registry);
}

} // namespace gc::yaml
