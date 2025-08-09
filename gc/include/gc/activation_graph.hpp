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
#include "gc/graph.hpp"

#include "common/grouped.hpp"

#include <iostream>
#include <memory>
#include <span>
#include <vector>


namespace gc {

using ActivationGraph = Graph<std::shared_ptr<ActivationNode>>;

struct ActivationGraphSourceTypes final
{
    std::vector<alg::id::Type> types;
    common::Grouped<EdgeInputEnd> destinations;

    auto operator==(const ActivationGraphSourceTypes&) const noexcept
        -> bool = default;
};

using EdgeInputEndSpan = std::span<const EdgeInputEnd>;
using EdgeInputEndVec = std::vector<EdgeInputEnd>;

auto operator<<(std::ostream& s, const ActivationGraphSourceTypes& source_types)
    -> std::ostream&;

auto generate_source(std::ostream& s,
                     const ActivationGraph& g,
                     alg::AlgorithmStorage& alg_storage,
                     const ActivationGraphSourceTypes& source_types = {},
                     EdgeInputEndSpan ignored_sources = {})
    -> void;

auto generate_source(std::ostream& s,
                     const ActivationGraph& g)
    -> void;

} // namespace gc
