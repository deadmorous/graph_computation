/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/activation_node_fwd.hpp"
#include "gc/algorithm_fwd.hpp"
#include "gc/port_values.hpp"
#include "gc/alg_type_literal.hpp"

#include "common/detail/hash.hpp"
#include "common/index_set.hpp"
#include "common/strong_vector.hpp"

#include <cassert>
#include <unordered_map>
#include <vector>

namespace gc {

constexpr inline auto max_ports = WeakPort{8};

struct Ports_Tag final {};

using InputPorts = common::IndexSet<InputPort>;

struct PortActivationAlgorithm final
{
    InputPorts required_inputs;
    alg::id::Statement activate;
};

using InputBindingVec =
    std::vector<alg::id::InputBinding>;

using PortActivationAlgorithmVec =
    common::StrongVector<PortActivationAlgorithm, InputPort>;

struct NodeActivationAlgorithms
{
    InputBindingVec input_bindings;
    PortActivationAlgorithmVec algorithms;
    alg::id::Vars context;
};

struct PrintableNodeActivationAlgorithms
{
    const NodeActivationAlgorithms& algs;
    const alg::AlgorithmStorage& alg_storage;
};

auto operator<<(std::ostream&, const PrintableNodeActivationAlgorithms&)
    -> std::ostream&;

using ExportedTypes =
    std::unordered_map<alg::TypeLiteral, alg::id::Type, common::detail::Hash>;

struct ActivationNode
{
    struct Meta
    {
        std::string_view type_name;
        bool dynamic_algorithm = false;
    };

    virtual ~ActivationNode() = default;

    virtual auto input_names() const -> InputNames = 0;

    virtual auto output_names() const -> OutputNames = 0;

    virtual auto exported_types(ExportedTypes&, alg::AlgorithmStorage&) const
        -> void
    {}

    virtual auto activation_algorithms(alg::AlgorithmStorage&) const
        -> NodeActivationAlgorithms = 0;

    virtual auto meta() const noexcept
        -> const Meta& = 0;

    auto input_count() const -> InputPortCount
    { return input_names().size(); }

    auto output_count() const -> OutputPortCount
    { return output_names().size(); }

    auto type_name() const noexcept
        -> std::string_view
    { return meta().type_name; }

    auto dynamic_algorithm() const noexcept
        -> bool
    { return meta().dynamic_algorithm; }

    auto exported_types(alg::AlgorithmStorage& alg_storage) const
        -> ExportedTypes
    {
        auto result = ExportedTypes{};
        exported_types(result, alg_storage);
        return result;
    }
};

} // namespace gc


// Use inside class declaration, after `public:`.
#define GCLIB_DECL_ACTIVATION_NODE_META(TypeName, ...)                      \
    static auto static_meta() noexcept                                      \
        -> const Meta&                                                      \
    {                                                                       \
        static auto meta = Meta{                                            \
            .type_name = #TypeName,                                         \
            .dynamic_algorithm = GCLIB_DEFAULT_A0_TO(false, ##__VA_ARGS__)  \
        };                                                                  \
        return meta;                                                        \
    }                                                                       \
                                                                            \
    auto meta() const noexcept                                              \
        -> const Meta& override                                             \
    { return static_meta(); }                                               \
    static_assert(true)
