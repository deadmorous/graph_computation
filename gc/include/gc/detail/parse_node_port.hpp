#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/edge.hpp"
#include "gc/node_port_tags.hpp"

#include <string_view>


namespace gc::detail {

auto parse_node_port(std::string_view, const NamedNodes&, Input_Tag)
    -> EdgeEnd;

auto parse_node_port(std::string_view, const NamedNodes&, Output_Tag)
    -> EdgeEnd;

} // namespace gc::detail