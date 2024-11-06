#pragma once

#include "gc/graph.hpp"
#include "gc/value.hpp"

#include "common/func_ref_fwd.hpp"
#include "common/grouped.hpp"

#include <stop_token>


namespace gc {

struct ComputationInstructions;
using ComputationInstructionsPtr = std::shared_ptr<ComputationInstructions>;
auto operator<<(std::ostream& s, const ComputationInstructions& instructions)
    -> std::ostream&;

// -----------

auto compile(const Graph& g)
    -> ComputationInstructionsPtr;

using Timestamp = uint64_t;

struct SourceInput final
{
    size_t node_index;
    size_t input_index;
    Value value;
};

using SourceInputVec = std::vector<SourceInput>;

struct ComputationResult final
{
    common::Grouped<Value> inputs;
    common::Grouped<Value> outputs;
    common::Grouped<Value> prev_source_outputs;
    std::vector<Timestamp> node_ts;
    Timestamp computation_ts{};
};

auto compute(ComputationResult& result,
             const Graph& g,
             const ComputationInstructions* instructions,
             const SourceInputVec& source_inputs = {})
    -> void;

using GraphProgress =
    common::FuncRef<void(uint32_t inode, double node_progress)>;

auto compute(ComputationResult& result,
             const Graph& g,
             const ComputationInstructions* instructions,
             const SourceInputVec& source_inputs,
             const std::stop_token& stoken,
             const GraphProgress& progress)
    -> bool;

struct Computation final
{
    Graph graph;
    ComputationInstructionsPtr instr;
    SourceInputVec source_inputs;
    ComputationResult result;
};

inline auto computation(Graph g, SourceInputVec source_inputs = {})
    -> Computation
{
    auto instr = compile(g);

    return {
        .graph = std::move(g),
        .instr = instr,
        .source_inputs = source_inputs };
}

inline auto compute(Computation& c)
    -> void
{ compute(c.result, c.graph, c.instr.get(), c.source_inputs); }

inline auto compute(Computation& c,
                    const std::stop_token& stoken,
                    const GraphProgress& progress)
    -> bool
{
    return compute(c.result,
                   c.graph,
                   c.instr.get(),
                   c.source_inputs,
                   stoken,
                   progress);
}

} // namespace gc
