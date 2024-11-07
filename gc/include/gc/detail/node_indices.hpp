#pragma once

#include "gc/node_fwd.hpp"

#include <unordered_map>


namespace gc::detail {

using NodeIndices =
    std::unordered_map<const gc::Node*, uint32_t>;

} // namespace gc::detail
