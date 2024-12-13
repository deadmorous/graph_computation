#pragma once

#include "gc/activation_node_fwd.hpp"
#include "gc/algorithm_fwd.hpp"
#include "gc/port_values.hpp"

#include "common/index_set.hpp"
#include "common/strong_vector.hpp"

#include <cassert>
#include <vector>

namespace gc {

constexpr inline auto max_ports = WeakPort{8};

struct Ports_Tag final {};

using InputPorts = common::IndexSet<InputPort>;

struct PortActivationAlgorithm final
{
    InputPorts required_inputs;
    alg::id::Statement activate;
    alg::id::Vars context;
};

using InputBindingVec =
    std::vector<alg::id::InputBinding>;

using PortActivationAlgorithmVec =
    common::StrongVector<PortActivationAlgorithm, InputPort>;

struct NodeActivationAlgorithms
{
    InputBindingVec input_bindings;
    PortActivationAlgorithmVec algorithms;
};

struct PrintableNodeActivationAlgorithms
{
    const NodeActivationAlgorithms& algs;
    const alg::AlgorithmStorage& alg_storage;
};

auto operator<<(std::ostream&, const PrintableNodeActivationAlgorithms&)
    -> std::ostream&;

struct ActivationNode
{
    virtual ~ActivationNode() = default;

    virtual auto input_names() const -> InputNames = 0;

    virtual auto output_names() const -> OutputNames = 0;

    virtual auto default_inputs(gc::InputValues result) const -> void = 0;

    virtual auto activation_algorithms(alg::AlgorithmStorage&) const
        -> NodeActivationAlgorithms = 0;

    auto input_count() const -> InputPortCount
    { return input_names().size(); }

    auto output_count() const -> OutputPortCount
    { return output_names().size(); }

};

} // namespace gc
