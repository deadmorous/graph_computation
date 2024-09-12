#include "gc/graph_computation.hpp"

#include "common/grouped.hpp"
#include "common/log.hpp"
#include "common/throw.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <set>

#include <quill/std/Set.h>


using namespace std::string_view_literals;

namespace gc {

namespace {

struct IndexEdgeEnd
{
    uint32_t inode;
    uint32_t port;

    auto operator<=>(const IndexEdgeEnd&) const noexcept
        -> std::strong_ordering = default;
};

using IndexEdge = std::array<IndexEdgeEnd, 2>;

auto operator<<(std::ostream& s, const IndexEdgeEnd& ee)
    -> std::ostream&
{ return s << '(' << ee.inode << ',' << ee.port << ')'; }

auto operator<<(std::ostream& s, const IndexEdge& e)
    -> std::ostream&
{ return s << '[' << e[0] << "->" << e[1] << ']'; }

} // anonymous namespace

struct ComputationInstructions
{
    common::Grouped<uint32_t>       nodes;
    common::Grouped<IndexEdge>      edges;
};

auto operator<<(std::ostream& s, const ComputationInstructions& instr)
    -> std::ostream&
{
    auto print_group = [&](const auto& grouped, uint32_t ig)
        { s << '(' << common::format_seq(group(grouped, ig)) << ')'; };

    auto n = group_count(instr.nodes);
    assert(group_count(instr.edges) + 1 == n);
    s << '{';
    print_group(instr.nodes, 0);
    for (uint32_t i=1; i<n; ++i)
    {
        s << " => ";
        print_group(instr.edges, i-1);
        s << " | ";
        print_group(instr.nodes, i);
    }
    return s;
}


// -----------

auto compile(const Graph& g)
    -> ComputationInstructionsPtr
{
    GC_LOG_INFO(
        "gc::compile: begin, nodes: {}, edges: {}",
        g.nodes.size(), g.edges.size());

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

    using IndexSet = std::set<uint32_t>;

    const auto node_ind_range =
        std::ranges::iota_view{uint32_t{}, static_cast<uint32_t>(nodes.size())};

    // Add all sources to the initial level of the graph
    auto level = IndexSet{};
    std::ranges::copy_if(
        node_ind_range,
        std::inserter(level, level.end()), has_all_inputs );
    if (level.empty())
        throw std::invalid_argument("Graph has no sources");

    auto known = level;

    // Map node-pointer-based edges to node-index-based ones
    auto edges = std::vector<IndexEdge>{};
    edges.reserve(g.edges.size());
    std::ranges::transform(
        g.edges,
        std::back_inserter(edges),
        [&](const Edge e) -> IndexEdge
        {
            auto transform_end = [&](const EdgeEnd& ee) -> IndexEdgeEnd
                { return { node_ind[ee.node], ee.port }; };
            return { transform_end(e[0]), transform_end(e[1]) };
        } );

    // We will further track connections of all inputs & outputs by edges
    using BitVec = std::vector<bool>;
    using BitVecs = std::vector<BitVec>;
    auto all_inputs = BitVecs{};
    all_inputs.reserve(nodes.size());
    for (size_t i=0, n=nodes.size(); i<n; ++i)
    {
        const auto* node = nodes[i];
        all_inputs.emplace_back(BitVec(node->input_count(), false));
    }

    // Check edges
    auto check_edge_end = [&](const IndexEdgeEnd& ee, bool input)
    {
        assert(ee.inode < nodes.size());
        const auto* node = nodes[ee.inode];
        if (input)
        {
            if (ee.port >= node->input_count())
                common::throw_<std::invalid_argument>(
                    "Edge end ", ee, " refers to a non-existent input port");
        }
        else
        {
            if (ee.port >= node->output_count())
                common::throw_<std::invalid_argument>(
                    "Edge end ", ee, " refers to a non-existent output port");
        }
    };

    auto check_edge = [&](const IndexEdge& e)
    {
        check_edge_end(e[0], false);
        check_edge_end(e[1], true);
    };

    std::ranges::for_each(edges, check_edge);

    auto result = std::make_shared<ComputationInstructions>();

    auto process_edge = [&](const IndexEdge& e)
    {
        const auto& e1 = e[1];
        assert (e1.inode < all_inputs.size());
        auto& ports = all_inputs[e1.inode];
        if (ports.at(e1.port))
            common::throw_<std::invalid_argument>(
                "Edge end ", e1,
                " is not the only one coming to the input port");
        ports[e1.port] = true;

        add_to_last_group(result->edges, e);
    };

    auto process_level = [&]
    {
        for (auto inode : level)
            add_to_last_group(result->nodes, inode);
        next_group(result->nodes);
    };

    auto process_level_edges = [&]
    {
        auto& edges = result->edges;
        next_group(edges);
        std::ranges::sort(group(edges, group_count(edges)-1));
    };

    process_level();

    // Process graph level by level
    while (known.size() < nodes.size())
    {
        GC_LOG_DEBUG("gc::compile: level: {}", level);
        auto next_level = IndexSet{};

        for (const auto& e : edges)
        {
            const auto& [e0, e1] = e;

            if (!level.contains(e0.inode))
                continue;

            if (known.contains(e1.inode))
                continue;

            process_edge(e);

            GC_LOG_DEBUG("gc::compile: {}", common::format(e));

            ++node_data[e1.inode].inputs_avail;
            if (has_all_inputs(e1.inode))
            {
                GC_LOG_DEBUG(
                    "gc::compile: node {} has all inputs ready", e1.inode);
                assert(!next_level.contains(e1.inode));
                next_level.insert(e1.inode);
            }
        }

        process_level_edges();

        if (next_level.empty())
        {
            auto unreachable = IndexSet{};
            std::ranges::copy_if(
                node_ind_range,
                std::inserter(unreachable, unreachable.end()),
                [&](uint32_t inode)
                    { return !known.contains(inode); } );

            common::throw_<std::invalid_argument>(
                "Graph is not connected. Unreachable nodes are ",
                common::format_seq(unreachable));
        }

        std::ranges::copy(next_level, std::inserter(known, known.end()));
        std::swap(level, next_level);
        process_level();
    }

    assert(result->nodes.values.size() == g.nodes.size());

    if(result->edges.values.size() != g.edges.size())
    {
        auto& sorted_edges = result->edges.values;
        auto is_edge_unprocessed = [&](const IndexEdge& e)
        {
            auto it = std::ranges::lower_bound(sorted_edges, e);
            return it == sorted_edges.end() || *it != e;
        };
        std::ranges::sort(sorted_edges);
        auto unprocessed_edges = std::set<IndexEdge>{};
        std::ranges::copy_if(
            edges,
            std::inserter(unprocessed_edges, unprocessed_edges.end()),
            is_edge_unprocessed);

        common::throw_<std::invalid_argument>(
            "The following edges are not processed because"
            " the graph has a cycle: ",
            common::format_seq(unprocessed_edges));
    }

    return result;
}

} // namespace gc
