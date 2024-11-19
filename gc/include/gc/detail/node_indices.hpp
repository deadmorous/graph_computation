#pragma once

#include "gc/node_fwd.hpp"
#include "gc/node_index.hpp"

#include <unordered_map>


namespace gc::detail {

using NodeIndices =
    std::unordered_map<const gc::Node*, gc::NodeIndex>;

} // namespace gc::detail
