/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/graph_evolution.hpp"
#include "gc_visual/parse_graph_binding.hpp"


namespace gc_visual {

auto parse_graph_evolution(
    const YAML::Node& config, const gc::ComputationContext&,
    const gc::ComputationGraph& graph,
    const gc::detail::NamedNodes<gc::ComputationNode>& node_map,
    std::span<std::string> input_names) -> GraphEvolution
{
    auto br = BindingResolver{graph, node_map, input_names};
    auto result = GraphEvolution{};

    auto feedback = config["feedback"];
    for (auto const& feedback_item : feedback)
    {
        auto source = parse_output_binding(br, feedback_item["source"]);
        auto sink = parse_input_binding(br, feedback_item["sink"]);
        result.feedback.push_back({
            .source_output = source,
            .sink_input = sink
        });
    }

    return result;
}

} // namespace gc_visual
