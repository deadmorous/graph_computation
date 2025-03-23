/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/detail/node_indices.hpp"
#include "gc/edge.hpp"
#include "gc/node_port_tags.hpp"

#include <string_view>


namespace gc::detail {

template <typename Node>
auto parse_node_port(std::string_view,
                     const NamedNodes<Node>&,
                     const NodeIndices<Node>& node_indices,
                     Input_Tag)
    -> EdgeInputEnd;

template <typename Node>
auto parse_node_port(std::string_view,
                     const NamedNodes<Node>&,
                     const NodeIndices<Node>& node_indices,
                     Output_Tag)
    -> EdgeOutputEnd;

} // namespace gc::detail
