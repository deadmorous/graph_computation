#pragma once

#include "gc/node.hpp"

#include "common/grouped.hpp"

#include <array>


namespace gc {

struct EdgeEnd final
{
    const Node*             node;
    uint32_t                port;
};

using Edge = std::array<EdgeEnd, 2>;

struct Graph final
{
    std::vector<NodePtr>    nodes;
    std::vector<Edge>       edges;
};

// -----------

struct ComputationInstructions;
using ComputationInstructionsPtr = std::shared_ptr<ComputationInstructions>;
auto operator<<(std::ostream& s, const ComputationInstructions& instructions)
    -> std::ostream&;

// -----------

auto compile(const Graph& g)
    -> ComputationInstructionsPtr;

struct ComputationResult
{
    common::Grouped<Value> inputs;
    common::Grouped<Value> outputs;
};

auto compute(ComputationResult& result,
             const Graph& g,
             const ComputationInstructions* instructions)
    -> void;

} // namespace gc
