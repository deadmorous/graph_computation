/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc/graph_computation.hpp"
#include "gc/computation_node.hpp"
#include "gc/strong_index.hpp"

#include "common/func_ref.hpp"
#include "common/index_range.hpp"
#include "common/log.hpp"
#include "common/throw.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <set>
#include <stdexcept>
#include <unordered_map>

#include <quill/std/Set.h>


using namespace std::string_view_literals;

namespace gc {

struct ComputationInstructions final
{
    // i-th group contains node indices of i-th graph level
    common::Grouped<NodeIndex>      nodes;

    // i-th group contains edges from nodes of level i
    // to nodes of level i+1
    common::Grouped<Edge>           edges;

    // i-th group contains indices of nodes supplying data
    // to the i-th node
    common::StrongGrouped<NodeIndex, NodeIndex, Index>  sources;
};

auto operator<<(std::ostream& s, const ComputationInstructions& instr)
    -> std::ostream&
{
    auto print_group = [&](const auto& grouped, auto ig)
        { s << '(' << common::format_seq(group(grouped, ig)) << ')'; };

    auto n = group_count(instr.nodes);
    assert(group_count(instr.edges) + 1 == n ||
           (n == 0 && group_count(instr.edges) == 0));
    s << '{';
    if (n > 0)
        print_group(instr.nodes, 0);
    for (decltype(n) i=1; i<n; ++i)
    {
        s << " => ";
        print_group(instr.edges, i-1);
        s << " | ";
        print_group(instr.nodes, i);
    }
    s << "}; [";
    auto* delim = "";
    for (auto i : group_indices(instr.sources))
    {
        s << delim;
        print_group(instr.sources, i);
        delim=",";
    }
    s << ']';
    return s;
}

// -----------

auto compile(const ComputationGraph& g, const SourceInputs& provided_inputs)
    -> std::pair<ComputationInstructionsPtr, SourceInputs>
{
    // GC_LOG_DEBUG(
    //     "gc::compile: begin, nodes: {}, edges: {}",
    //     g.nodes.size(), g.edges.size());

    auto result = std::make_shared<ComputationInstructions>();

    if (g.edges.empty() && g.nodes.empty())
        return { std::move(result), {} };

    // Create a vector of raw pointers to graph nodes
    // TODO: Get rid of it?
    auto nodes = common::StrongVector<const ComputationNode*, NodeIndex>{};
    nodes.reserve(g.nodes.size());
    std::ranges::transform(
        g.nodes,
        std::back_inserter(nodes),
        [](const ComputationNodePtr& node) { return node.get(); });

    // Map node pointers to their indices in g.nodes
    std::unordered_map<const ComputationNode*, NodeIndex> node_ind;
    for (auto i : nodes.index_range())
        node_ind[nodes[i]] = i;

    // Check edges
    auto check_edge_end = [&]<typename Tag>(const EdgeEnd<Tag>& ee)
    {
        if(!nodes.index_range().contains(ee.node))
            common::throw_<std::out_of_range>(
                "Edge end ", ee, " refers to a non-existent node");

        const auto* node = nodes[ee.node];
        if constexpr (std::same_as<Tag, Input_Tag>)
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

    auto check_edge = [&](const Edge& e)
    {
        check_edge_end(e.from);
        check_edge_end(e.to);
    };

    std::ranges::for_each(g.edges, check_edge);

    using BitVec = common::StrongVector<bool, InputPort>;

    // Obtain information on node input and output counts.
    struct NodeData
    {
        InputPortCount input_count{};
        OutputPortCount output_count{};
        InputPortCount inputs_avail{};

        // We will further track connections of all inputs & outputs by edges
        BitVec connected_inputs{};

        // Number of inputs connected to outputs of other nodes with graph edges
        InputPortCount connected_input_count{};
    };
    auto node_data = common::StrongVector<NodeData, NodeIndex>{};
    node_data.reserve(nodes.size());
    std::ranges::transform(
        nodes,
        std::back_inserter(node_data),
        [](const ComputationNode* node) -> NodeData
            { return { node->input_count(),
                       node->output_count(),
                       common::Zero,
                       BitVec(node->input_count(), false) }; } );

    for (const auto& e : g.edges)
        ++node_data.at(e.to.node).connected_input_count;

    auto has_all_inputs =
        [&](gc::NodeIndex node_index)
        {
            auto& d = node_data[node_index];
            return d.inputs_avail == d.connected_input_count;
        };

    using IndexSet = std::set<gc::NodeIndex>;

    const auto node_ind_range =
        std::ranges::iota_view{WeakNodeIndex{}, nodes.size().v} |
        std::views::transform( [](WeakNodeIndex x){ return NodeIndex{x}; } );

    // Add all sources to the initial level of the graph
    auto level = IndexSet{};
    std::ranges::copy_if(
        node_ind_range,
        std::inserter(level, level.end()),
        has_all_inputs);
    if (level.empty())
        throw std::invalid_argument("Graph has no sources");

    auto known = level;

    auto process_edge = [&](const Edge& e)
    {
        const auto& e1 = e.to;
        assert (node_data.index_range().contains(e1.node));
        auto& ports = node_data[e1.node].connected_inputs;
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
    while (known.size() < nodes.size().v)
    {
        // GC_LOG_DEBUG("gc::compile: level: {}", level);
        auto next_level = IndexSet{};

        for (const auto& e : g.edges)
        {
            const auto& [e0, e1] = e;

            if (!level.contains(e0.node))
                continue;

            if (known.contains(e1.node))
                continue;

            process_edge(e);

            // GC_LOG_DEBUG("gc::compile: {}", common::format(e));

            ++node_data[e1.node].inputs_avail;
            if (has_all_inputs(e1.node))
            {
                // GC_LOG_DEBUG(
                //     "gc::compile: node {} has all inputs ready", e1.inode);
                assert(!next_level.contains(e1.node));
                next_level.insert(e1.node);
            }
        }

        process_level_edges();

        if (next_level.empty())
        {
            auto unreachable = IndexSet{};
            std::ranges::copy_if(
                node_ind_range,
                std::inserter(unreachable, unreachable.end()),
                [&](gc::NodeIndex inode)
                    { return !known.contains(inode); } );

            common::throw_<std::invalid_argument>(
                "Graph is not connected. Unreachable nodes are ",
                common::format_seq(unreachable));
        }

        std::ranges::copy(next_level, std::inserter(known, known.end()));
        std::swap(level, next_level);
        process_level();
    }

    assert(result->nodes.values.size() == g.nodes.size().v);

    if(result->edges.values.size() != g.edges.size())
    {
        auto& sorted_edges = result->edges.values;
        auto is_edge_unprocessed = [&](const Edge& e)
        {
            auto it = std::ranges::lower_bound(sorted_edges, e);
            return it == sorted_edges.end() || *it != e;
        };
        std::ranges::sort(sorted_edges);
        auto unprocessed_edges = std::set<Edge>{};
        std::ranges::copy_if(
            g.edges,
            std::inserter(unprocessed_edges, unprocessed_edges.end()),
            is_edge_unprocessed);

        common::throw_<std::invalid_argument>(
            "The following edges are not processed because"
            " the graph has a cycle: ",
            common::format_seq(unprocessed_edges));
    }

    // Build source map
    using SimpleEdge = std::array<gc::NodeIndex, 2>; // to -> from
    auto simple_edges = std::vector<SimpleEdge>{};
    for (const auto& e : g.edges)
        simple_edges.push_back({e.to.node, e.from.node});
    std::sort(simple_edges.begin(), simple_edges.end());
    simple_edges.resize(
        std::unique(simple_edges.begin(), simple_edges.end()) -
        simple_edges.begin());

    {
        auto ie = size_t{};
        for (auto i : nodes.index_range())
        {
            for (; ie<simple_edges.size() && simple_edges[ie][0] == i; ++ie)
                add_to_last_group(result->sources, simple_edges[ie][1]);
            next_group(result->sources);
        }
    }

    // Build source inputs.
    // Start with provided inputs and augment with any missing ones.
    auto source_inputs = provided_inputs;

    // Build a sorted vector
    // of all destinations of inputs provided.
    auto input_dst = provided_inputs.destinations.values;
    std::ranges::sort(input_dst);

    auto input_provided = [&](const EdgeInputEnd& dst)
    {
        auto it = std::ranges::lower_bound(input_dst, dst);
        return it != input_dst.end() && *it == dst;
    };

    // Check that the destinations of inputs provided are valid
    for (const auto& dst : input_dst)
    {
        if(!nodes.index_range().contains(dst.node))
            common::throw_<std::out_of_range>(
                "Source input destination ", dst,
                " refers to a non-existent node");

        if (dst.port >= InputPort{} + nodes[dst.node]->input_count())
            common::throw_<std::invalid_argument>(
                "Source input destination ", dst,
                " refers to a non-existent input port");
    }

    // Check that provided inputs do not specify destinations
    // coincident with any edge targets. Add inputs that were not
    // provided.
    for (auto i : nodes.index_range())
    {
        const auto& nd = node_data[i];
        const auto* node = nodes[i];
        using InputValueVec = common::StrongVector<Value, InputPort>;
        auto default_inputs = InputValueVec(node->input_count());
        nodes[i]->default_inputs(default_inputs);
        for (auto port : default_inputs.index_range())
        {
            auto dst = EdgeInputEnd{i, port};
            auto provided = input_provided(dst);
            if (nd.connected_inputs[port])
            {
                if (provided)
                    common::throw_<std::invalid_argument>(
                        "Input for destination ", dst, " is provided,"
                        " but an output of another node is"
                        " connected to the same destination.");
                continue;
            }

            if (provided)
                continue;

            source_inputs.values.push_back(default_inputs[port]);
            add_to_last_group(source_inputs.destinations, EdgeInputEnd{i,port});
            next_group(source_inputs.destinations);
        }
    }

    return { std::move(result), std::move(source_inputs) };
}

auto compute(ComputationResult& result,
             const ComputationGraph& g,
             const ComputationInstructions* instructions,
             const SourceInputs& source_inputs)
    -> void
{ compute(result, g, instructions, source_inputs, {}, {}); }

auto compute(ComputationResult& result,
             const ComputationGraph& g,
             const ComputationInstructions* instructions,
             const SourceInputs& source_inputs,
             const std::stop_token& stoken,
             const GraphProgress& progress)
    -> bool
{
    auto input_count =
       [](const gc::ComputationNode& node){ return node.input_count(); };
    auto output_count =
       [](const gc::ComputationNode& node){ return node.output_count(); };

    if (result.outputs.v.values.empty())
    {
        // Allocate result grouped data
        auto fill = [&]<typename T, typename IO, typename II>(
                        common::StrongGrouped<T, IO, II>& grouped, auto count)
        {
            grouped = {};
            for (const auto& node : g.nodes)
            {
                for (auto i : common::index_range<II>(count(*node)))
                    common::add_to_last_group(grouped, T{});
                common::next_group(grouped);
            }
        };

        fill(result.inputs, input_count);
        fill(result.outputs, output_count);
        fill(result.prev_source_outputs, output_count);
        result.node_ts =
            common::StrongVector<Timestamp, NodeIndex>(g.nodes.size(), 0);
        result.computation_ts = 0;
    }

    else
    {
        // Validate result grouped data
        auto check = [&](auto& grouped, auto count)
        {
            assert (group_count(grouped) == g.nodes.size());
            for (auto inode : g.nodes.index_range())
                assert(group(grouped, inode).size() == count(*g.nodes[inode]));
        };

        check(result.inputs, input_count);
        check(result.outputs, output_count);
        check(result.prev_source_outputs, output_count);
        assert(result.node_ts.size() == g.nodes.size());
    }

    ++result.computation_ts;

    auto source_updated =
        common::StrongVector<bool, NodeIndex>(g.nodes.size(), false);

    // Set external inputs
    for (size_t i=0, n=source_inputs.values.size(); i<n; ++i)
    {
        const auto& value = source_inputs.values[i];

        for (auto d : group(source_inputs.destinations, i))
        {
            if (!g.nodes.index_range().contains(d.node))
                common::throw_<std::out_of_range>(
                    "Source input ", d, " refers to an inexistent graph node");
            auto node_inputs = group(result.inputs, d.node);
            if (!node_inputs.index_range().contains(d.port))
                common::throw_<std::out_of_range>(
                    "Source input ", d, " refers to an inexistent input port");
            auto& node_input = node_inputs[d.port];
            if (node_input != value)
            {
                node_input = value;
                source_updated[d.node] = true;
            }
        }
    }

    auto nlevels = group_count(instructions->nodes);
    for (auto level=0; level<nlevels; ++level)
    {
        if (level > 0)
        {
            for (const auto& e : group(instructions->edges, level-1))
            {
                const auto& [e0, e1] = e;
                group(result.inputs, e1.node)[e1.port] =
                    group(result.outputs, e0.node)[e0.port];
            }
        }

        for (auto inode : group(instructions->nodes, level))
        {
            Timestamp upstream_ts;

            // Check if a source input of the node `inode` has been updated
            auto node_ts = result.node_ts[inode];
            auto upstream_updated = source_updated[inode] ||
                                    node_ts == Timestamp{};
            if (upstream_updated)
                upstream_ts = result.computation_ts;
            else
            {
                // If no source inputs have been updated,
                // also check if any of source nodes has been updated
                upstream_ts = node_ts;
                for (auto i : group(instructions->sources, inode))
                    upstream_ts = std::max(upstream_ts, result.node_ts[i]);
                upstream_updated = node_ts < upstream_ts;
            }

            if (!upstream_updated)
                continue;

            auto node_progress =
                [&](double progress_value)
            { progress(inode, progress_value); };
            auto node_progress_func = progress
                ? NodeProgress{ &node_progress }
                : NodeProgress{};

            auto computed =
                g.nodes[inode]->compute_outputs(
                    group(result.outputs, inode),
                    group(result.inputs, inode),
                    stoken, node_progress_func);

            if (!computed)
                return false;

            result.node_ts[inode] = upstream_ts;
        }
    }

    return true;
}

} // namespace gc
