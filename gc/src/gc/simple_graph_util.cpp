/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/simple_graph_util.hpp"

#include <queue>
#include <ranges>


namespace gc {
namespace {

struct CompareEdge final
{
    using Edge = GraphStructure::Edge;

    auto operator()(const Edge& a, const Edge& b) const noexcept
    { return a.from < b.from; }

    auto operator()(const NodeIndex& a, const Edge& b) const noexcept
    { return a < b.from; }

    auto operator()(const Edge& a, const NodeIndex& b) const noexcept
    { return a.from < b; }

    auto operator()(const NodeIndex& a, const NodeIndex& b) const noexcept
    { return a < b; }

};

} // anonymous namespace

auto topological_sort(const GraphStructure& gs)
    -> std::vector<NodeIndex>
{
    auto result = std::vector<NodeIndex>{};
    result.reserve(gs.node_count.v);

    if (gs.node_count == common::Zero && gs.edges.empty())
        return result;

    auto input_counts =
        common::StrongVector<uint32_t, NodeIndex>(gs.node_count, 0);

    for (const auto& e : gs.edges)
        ++input_counts.at(e.to);

    auto queue = std::queue<NodeIndex>{};

    for (auto index : input_counts.index_range())
        if (input_counts[index] == 0)
            queue.push(index);

    constexpr auto cmp = CompareEdge{};

    auto sorted_edges = gs.edges;
    std::ranges::sort(sorted_edges, cmp);

    while (!queue.empty())
    {
        auto index = queue.front();
        queue.pop();
        result.push_back(index);
        for (const auto& e : std::ranges::equal_range(sorted_edges, index, cmp))
            if (--input_counts.at(e.to) == 0)
                queue.push(e.to);
    }

    if (result.size() != gs.node_count.v)
        common::throw_(
            "Graph structure does not describe a DAG"
            " - topological_sort failed");

    return result;
}

} // namespace gc
