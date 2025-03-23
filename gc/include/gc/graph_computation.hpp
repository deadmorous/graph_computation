/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/computation_graph.hpp"
#include "gc/source_inputs.hpp"
#include "gc/value.hpp"

#include "common/func_ref_fwd.hpp"
#include "common/strong_grouped.hpp"
#include "common/strong_vector.hpp"

#include <stop_token>


namespace gc {

struct ComputationInstructions;
using ComputationInstructionsPtr = std::shared_ptr<ComputationInstructions>;
auto operator<<(std::ostream& s, const ComputationInstructions& instructions)
    -> std::ostream&;

// -----------

auto compile(const ComputationGraph& g,
             const SourceInputs& provided_inputs = {})
    -> std::pair<ComputationInstructionsPtr, SourceInputs>;

using Timestamp = uint64_t;

struct ComputationResult final
{
    common::StrongGrouped<Value, NodeIndex, InputPort> inputs;
    common::StrongGrouped<Value, NodeIndex, OutputPort> outputs;
    common::StrongGrouped<Value, NodeIndex, OutputPort> prev_source_outputs;
    common::StrongVector<Timestamp, NodeIndex> node_ts;
    Timestamp computation_ts{};
};

auto compute(ComputationResult& result,
             const ComputationGraph& g,
             const ComputationInstructions* instructions,
             const SourceInputs& source_inputs = {})
    -> void;

using GraphProgress =
    common::FuncRef<void(NodeIndex inode, double node_progress)>;

auto compute(ComputationResult& result,
             const ComputationGraph& g,
             const ComputationInstructions* instructions,
             const SourceInputs& source_inputs,
             const std::stop_token& stoken,
             const GraphProgress& progress)
    -> bool;

struct Computation final
{
    ComputationGraph graph;
    ComputationInstructionsPtr instr;
    SourceInputs source_inputs;
    ComputationResult result;
};

inline auto computation(ComputationGraph g, const SourceInputs& provided_inputs)
    -> Computation
{
    auto [instr, source_inputs] = compile(g, provided_inputs);

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
