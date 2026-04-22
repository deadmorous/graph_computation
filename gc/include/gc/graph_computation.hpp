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

#include "mpk/mix/func_ref/fwd.hpp"
#include "mpk/mix/strong/grouped.hpp"
#include "mpk/mix/strong/vector.hpp"
#include "mpk/mix/util/detail/hash.hpp"

#include <stop_token>
#include <unordered_set>


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
    mpk::mix::StrongGrouped<Value, NodeIndex, InputPort> inputs;
    mpk::mix::StrongGrouped<Value, NodeIndex, OutputPort> outputs;
    mpk::mix::StrongGrouped<Value, NodeIndex, OutputPort> prev_source_outputs;
    mpk::mix::StrongVector<Timestamp, NodeIndex> node_ts;
    Timestamp computation_ts{};

    // Used when there is a feedback determining state evolution
    std::unordered_set<EdgeInputEnd, mpk::mix::detail::Hash> updated_inputs;
};

auto compute(ComputationResult& result,
             const ComputationGraph& g,
             const ComputationInstructions* instructions,
             const SourceInputs& source_inputs = {})
    -> void;

using GraphProgress =
    mpk::mix::FuncRef<void(NodeIndex inode, double node_progress)>;

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
