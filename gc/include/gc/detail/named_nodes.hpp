#pragma once

#include "gc/node_fwd.hpp"

#include <string>
#include <unordered_map>


namespace gc::detail {

using NamedNodes =
    std::unordered_map<std::string, Node*>;

} // namespace gc::detail
