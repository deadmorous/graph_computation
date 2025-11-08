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

auto BindingResolver::input_index(const std::string& input_name) const
    -> size_t
{
    auto it = std::find(input_names_.begin(), input_names_.end(), input_name);
    if (it == input_names_.end())
        common::throw_<std::invalid_argument>(
            "Input with name '", input_name, " is not found");
    return it - input_names_.begin();
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
    auto input_name = item_node.as<std::string>();
    auto input_index = resolver.input_index(input_name);
    return { .input = input_index };
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
    auto result = binding.input_name;
    if (!binding.param_spec.path.empty())
        result += common::format(binding.param_spec.path);
    return result;
}

auto parse_param_binding(const BindingResolver& resolver,
                         const YAML::Node& item_node)
    -> ParamBinding
{
    // Resolve parameter binding
    auto input_name = item_node["bind"].as<std::string>();

    auto path = gc::ValuePath{};
    auto path_pos = input_name.find_first_of('/');
    if (path_pos != std::string::npos)
    {
        path = gc::ValuePath::from_string(input_name.substr(path_pos));
        input_name = input_name.substr(0, path_pos);
    }

    auto input = resolver.input_index(input_name);

    return { { input, path }, std::move(input_name) };
}

} // namespace gc_visual
