#pragma once

#include "gc/simple_graph.hpp"

#include "common/detail/hash.hpp"
#include "common/grouped.hpp"
#include "common/throw.hpp"

#include <algorithm>
#include <unordered_map>


namespace gc {

struct GraphStructure final
{
    using Edge = SimpleEdge<NodeIndex>;
    using EdgeVec = std::vector<Edge>;

    NodeCount node_count;
    EdgeVec edges;
};

template <typename Node>
auto graph_structure(const SimpleGraph<Node>& g)
    ->GraphStructure
{
    std::unordered_map<Node, NodeIndex, common::detail::Hash> indices;
    for (auto index : g.nodes.index_range())
    {
        const auto& node = g.nodes[index];
        if (indices.contains(node))
            common::throw_(
                "Node ", node, " is encountered in the graph more than once");
        indices[node] = index;
    }

    auto edges = GraphStructure::EdgeVec{};
    edges.reserve(g.edges.size());
    for (const auto& e : g.edges)
        edges.push_back(simple_edge(indices.at(e.from), indices.at(e.to)));

    return { g.nodes.size(), std::move(edges) };
}

auto topological_sort(const GraphStructure& gs)
    -> std::vector<NodeIndex>;

template <typename Node>
auto topological_sort(const SimpleGraph<Node>& g)
    -> std::vector<Node>
{
    auto indices = topological_sort(graph_structure(g));
    auto result = std::vector<Node>{};
    result.reserve(indices.size());
    std::transform(indices.begin(),
                   indices.end(),
                   back_inserter(result),
                   [&g](NodeIndex index) { return g.nodes.at(index); });
    return result;
}

} // namespace gc
