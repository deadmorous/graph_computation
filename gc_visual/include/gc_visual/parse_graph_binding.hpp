/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/graph_bindings.hpp"

#include "gc/detail/computation_node_indices.hpp"
#include "gc/detail/named_nodes.hpp"
#include "gc/computation_graph.hpp"

#include <yaml-cpp/yaml.h>

#include <span>


namespace gc_visual {

class BindingResolver final
{
public:
    BindingResolver(
        const gc::ComputationGraph& graph,
        const gc::detail::NamedNodes<gc::ComputationNode>& node_map,
        std::span<const std::string> input_names);

    auto node_index(const gc::ComputationNode* node) const
        -> gc::NodeIndex;

    auto io_spec(const std::string& io_name) const
        -> gc::IoSpec;

    auto node_map() const noexcept
        -> const gc::detail::NamedNodes<gc::ComputationNode>&;

    auto input_names() const noexcept
        -> std::span<const std::string>;

    auto node_indices() const noexcept
        -> const gc::detail::ComputationNodeIndices&;

private:
    const gc::detail::NamedNodes<gc::ComputationNode>& node_map_;
    std::span<const std::string> input_names_;
    gc::detail::ComputationNodeIndices node_indices_;
};

auto parse_input_binding(const BindingResolver& resolver,
                         const YAML::Node& item_node)
    -> InputBinding;

auto parse_output_binding(const BindingResolver& resolver,
                          const YAML::Node& item_node)
    -> OutputBinding;

auto param_binding_label(const ParamBinding& binding)
    -> std::string;

auto parse_param_binding(const BindingResolver& resolver,
                         std::string io_name)
    -> ParamBinding;

auto parse_param_binding(const BindingResolver& resolver,
                         const YAML::Node& item_node)
    -> ParamBinding;

} // namespace gc_visual
