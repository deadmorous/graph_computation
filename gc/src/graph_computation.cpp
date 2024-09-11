#include "gc/graph_computation.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>


namespace gc {

namespace {



} // anonymous namespace

struct ComputationInstructions
{};

auto compile(const Graph& g)
    -> ComputationInstructionsPtr
{
    // View graph nodes as raw pointers
    auto nodes = std::ranges::transform_view(
        g.nodes,
        [](const NodePtr& node) { return node.get(); } );

    // Map node pointers to their indices in g.nodes
    std::unordered_map<const Node*, uint32_t> node_ind;
    for (size_t i=0, n=nodes.size(); i<n; ++i)
        node_ind[nodes[i]] = i;

    // Obtain information on node input and output counts.
    struct NodeData
    {
        uint32_t input_count{};
        uint32_t output_count{};
        uint32_t inputs_avail{};
    };
    auto node_data = std::vector<NodeData>{};
    node_data.reserve(nodes.size());
    std::ranges::transform(
        nodes,
        std::back_inserter(node_data),
        [](const Node* node) -> NodeData
            { return { node->input_count(), node->output_count(), 0 }; } );

    auto has_all_inputs =
        [&](uint32_t node_index)
        {
            auto& d = node_data[node_index];
            return d.inputs_avail == d.input_count;
        };

    using IndexSet = std::unordered_set<uint32_t>;

    auto known = IndexSet{};

    // Add all sources to the initial level of the graph
    auto level = IndexSet{};
    std::ranges::copy_if(
        std::ranges::iota_view{uint32_t{}, static_cast<uint32_t>(nodes.size())},
        std::inserter(level, level.end()), has_all_inputs );
    if (level.empty())
        throw std::invalid_argument("Graph has no sources");

    // Map node-pointer-based edges to node-index-based ones
    struct EE
    {
        uint32_t inode;
        uint32_t port;
    };
    using E = std::array<EE, 2>;
    auto edges = std::vector<E>{};
    edges.reserve(g.edges.size());
    std::ranges::transform(
        g.edges,
        std::back_inserter(edges),
        [&](const Edge e) -> E
        {
            auto transform_end = [&](const EdgeEnd& ee) -> EE
                { return { node_ind[ee.node], ee.port }; };
            return { transform_end(e[0]), transform_end(e[1]) };
        } );

    // Process graph level by level
    while (known.size() < nodes.size())
    {
        auto next_level = IndexSet{};

        for (const auto& e : edges)
        {
            const auto& [e0, e1] = e;

            if (!level.contains(e0.inode))
                continue;

            if (known.contains(e1.inode))
                continue;

            ++node_data[e1.inode].inputs_avail;
            if (has_all_inputs(e1.inode))
            {
                assert(!next_level.contains(e1.inode));
                next_level.insert(e1.inode);
            }
        }
        if (next_level.empty())
            throw std::invalid_argument("Graph is not connected");

        std::ranges::copy(next_level, std::inserter(known, known.end()));
        std::swap(level, next_level);
    }

    return std::make_shared<ComputationInstructions>(); // TODO
}

} // namespace gc
