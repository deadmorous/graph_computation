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

#include "gc/computation_context_fwd.hpp"
#include "gc/computation_graph.hpp"
#include "gc/detail/named_nodes.hpp"

#include <yaml-cpp/yaml.h>

#include <span>
#include <vector>


namespace gc_visual {

struct EvolutionFeedback final
{
    OutputBinding source_output;
    InputBinding sink_input;
};

using EvolutionFeedbackVec = std::vector<EvolutionFeedback>;

struct GraphEvolution final
{
    EvolutionFeedbackVec feedback;
};

auto parse_graph_evolution(
    const YAML::Node& config, const gc::ComputationContext& context,
    const gc::ComputationGraph& graph,
    const gc::detail::NamedNodes<gc::ComputationNode>& node_map,
    std::span<std::string> input_names) -> GraphEvolution;

} // namespace gc_visual
