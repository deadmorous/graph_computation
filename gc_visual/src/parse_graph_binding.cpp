/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/parse_graph_binding.hpp"

#include "gc/computation_node.hpp"
#include "gc/detail/parse_node_port.hpp"

#include "common/format.hpp"
#include "common/throw.hpp"

#include <span>


namespace gc_visual {

BindingResolver::BindingResolver(
    const gc::ComputationGraph& graph,
    const gc::detail::NamedNodes<gc::ComputationNode>& node_map,
    std::span<const std::string> input_names) :
    node_map_{node_map},
    input_names_{input_names}
{
    for (uint32_t index=0; const auto& node: graph.nodes)
        node_indices_[node.get()] = gc::NodeIndex{index++};
}

auto BindingResolver::node_index(const gc::ComputationNode* node) const
    -> gc::NodeIndex
{ return node_indices_.at(node); }

auto BindingResolver::io_spec(const std::string& io_name) const
    -> gc::IoSpec
{
    auto dot_pos = io_name.find_first_of('.');

    if (dot_pos == std::string::npos)
    {
        // No dot in the name means it's an input name
        auto it =
            std::find(input_names_.begin(), input_names_.end(), io_name);

        if (it == input_names_.end())
            common::throw_<std::invalid_argument>(
                "Input with name '", io_name, "' is not found");

        return gc::ExternalInputSpec{
            .input = size_t(it - input_names_.begin()) };
    }
    else
    {
        // Dot in the name means it's node.output_port
        auto node_name = io_name.substr(0, dot_pos);
        auto it_node = node_map_.find(node_name);

        if (it_node == node_map_.end())
            common::throw_<std::invalid_argument>(
                "Output with name '", io_name,
                "' is not found - no node '", node_name, "'");

        const auto* node = it_node->second;
        auto output_names = node->output_names();
        auto port_name = io_name.substr(dot_pos+1);
        auto it_port =
            std::ranges::find(output_names, std::string_view{port_name});

        if (it_port == output_names.end())
            common::throw_<std::invalid_argument>(
                "Output with name '", io_name,
                "' is not found - node '", node_name,
                "' has no port '", port_name, "'");

        auto index = node_index(node);
        auto port = gc::OutputPort(it_port - output_names.begin());
        return gc::NodeOutputSpec{ .output = {index, port} };
    }
}

auto BindingResolver::node_map() const noexcept
    -> const gc::detail::NamedNodes<gc::ComputationNode>&
{ return node_map_; }

auto BindingResolver::input_names() const noexcept
    -> std::span<const std::string>
{ return input_names_; }

auto BindingResolver::node_indices() const noexcept
    -> const gc::detail::ComputationNodeIndices&
{ return node_indices_; }

auto parse_input_binding(const BindingResolver& resolver,
                         const YAML::Node& item_node)
    -> InputBinding
{
    auto result = InputBinding{};
    result.inputs.reserve(item_node.size());
    for (auto input : item_node)
    {
        auto edge_end =
            gc::detail::parse_node_port(
                input.as<std::string>(),
                resolver.node_map(),
                resolver.node_indices(),
                gc::Input);
        result.inputs.push_back(edge_end);
    }
    return result;
}

auto parse_output_binding(const BindingResolver& resolver,
                          const YAML::Node& item_node)
    -> OutputBinding
{
    auto edge_end =
        gc::detail::parse_node_port(
            item_node.as<std::string>(),
            resolver.node_map(),
            resolver.node_indices(),
            gc::Output);
    return { .output = edge_end };
}

auto param_binding_label(const ParamBinding& binding)
    -> std::string
{
    auto result = binding.io_name;
    if (!binding.param_spec.path.empty())
        result += common::format(binding.param_spec.path);
    return result;
}

auto parse_param_binding(const BindingResolver& resolver,
                         std::string io_name)
    -> ParamBinding
{
    auto path = gc::ValuePath{};
    auto path_pos = io_name.find_first_of('/');
    if (path_pos != std::string::npos)
    {
        path = gc::ValuePath::from_string(io_name.substr(path_pos));
        io_name = io_name.substr(0, path_pos);
    }

    auto io = resolver.io_spec(io_name);

    return { { io, path }, std::move(io_name) };
}

auto parse_param_binding(const BindingResolver& resolver,
                         const YAML::Node& item_node)
    -> ParamBinding
{
    // Resolve parameter binding
    auto io_name = item_node["bind"].as<std::string>();
    return parse_param_binding(resolver, io_name);
}

} // namespace gc_visual
