#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/detail/node_indices.hpp"
#include "gc/edge.hpp"
#include "gc/node_port_tags.hpp"

#include <string_view>


namespace gc::detail {

auto parse_node_port(std::string_view,
                     const NamedNodes&,
                     const NodeIndices& node_indices,
                     Input_Tag)
    -> EdgeInputEnd;

auto parse_node_port(std::string_view,
                     const NamedNodes&,
                     const NodeIndices& node_indices,
                     Output_Tag)
    -> EdgeOutputEnd;

} // namespace gc::detail
