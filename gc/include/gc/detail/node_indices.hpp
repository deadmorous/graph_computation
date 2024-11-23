#pragma once

#include "gc/node_index.hpp"

#include <unordered_map>


namespace gc::detail {

template <typename Node>
using NodeIndices = std::unordered_map<const Node*, NodeIndex>;

} // namespace gc::detail
