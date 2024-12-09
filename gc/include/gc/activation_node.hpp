#pragma once

#include "gc/activation_node_fwd.hpp"
#include "gc/algorithm_fwd.hpp"
#include "gc/port_values.hpp"
#include "gc/value_fwd.hpp"

#include "common/const_name_span.hpp"
#include "common/index_set.hpp"

#include <cassert>
#include <bitset>

namespace gc {

constexpr inline auto max_ports = WeakPort{8};

struct Ports_Tag final {};

using InputPorts = common::IndexSet<InputPort>;

struct PortActivationAlgorithm final
{
    InputPorts required_inputs;
    alg::id::Statement activate;
    alg::id::Var context;
};

using ActivationAlgorithmsResult =
    common::StrongSpan<PortActivationAlgorithm, InputPort>;

struct ActivationNode
{
    virtual ~ActivationNode() = default;

    virtual auto input_names() const -> InputNames = 0;

    virtual auto output_names() const -> OutputNames = 0;

    virtual auto default_inputs(gc::InputValues result) const -> void = 0;

    virtual auto activation_algorithms(ActivationAlgorithmsResult,
                                       alg::AlgorithmStorage&) const
        -> void = 0;

    auto input_count() const -> InputPortCount
    { return input_names().size(); }

    auto output_count() const -> OutputPortCount
    { return output_names().size(); }

};

} // namespace gc
