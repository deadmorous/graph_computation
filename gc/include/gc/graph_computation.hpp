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

struct SourceInput final
{
    size_t node;
    size_t port;
    Value value;

    auto operator==(const SourceInput&) const noexcept -> bool = default;
};
auto operator<<(std::ostream& s, const SourceInput& source_input)
    -> std::ostream&;

using SourceInputVec = std::vector<SourceInput>;

auto compile(const Graph& g)
    -> std::pair<ComputationInstructionsPtr, SourceInputVec>;

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

inline auto computation(Graph g)
    -> Computation
{
    auto [instr, source_inputs] = compile(g);

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
