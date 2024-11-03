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
             const ComputationInstructions* instructions)
    -> void;

using GraphProgress =
    common::FuncRef<void(uint32_t inode, double node_progress)>;

auto compute(ComputationResult& result,
             const Graph& g,
             const ComputationInstructions* instructions,
             const std::stop_token& stoken,
             const GraphProgress& progress)
    -> bool;

struct Computation final
{
    Graph graph;
    ComputationInstructionsPtr instr;
    ComputationResult result;
};

inline auto computation(Graph g)
    -> Computation
{
    auto instr = compile(g);

    return {
        .graph = std::move(g),
        .instr = instr };
}

inline auto compute(Computation& computation)
    -> void
{ compute(computation.result, computation.graph, computation.instr.get()); }

inline auto compute(Computation& computation,
                    const std::stop_token& stoken,
                    const GraphProgress& progress)
    -> bool
{
    return compute(computation.result,
                   computation.graph,
                   computation.instr.get(),
                   stoken,
                   progress);
}

} // namespace gc
