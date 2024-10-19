#pragma once

#include "gc/graph.hpp"
#include "gc/value.hpp"

#include "common/grouped.hpp"


namespace gc {

struct ComputationInstructions;
using ComputationInstructionsPtr = std::shared_ptr<ComputationInstructions>;
auto operator<<(std::ostream& s, const ComputationInstructions& instructions)
    -> std::ostream&;

// -----------

auto compile(const Graph& g)
    -> ComputationInstructionsPtr;

struct ComputationResult final
{
    common::Grouped<Value> inputs;
    common::Grouped<Value> outputs;
};

auto compute(ComputationResult& result,
             const Graph& g,
             const ComputationInstructions* instructions)
    -> void;

} // namespace gc
