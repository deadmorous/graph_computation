#pragma once

#include <string>
#include <unordered_map>


namespace gc::detail {

template <typename Node>
using NamedNodes = std::unordered_map<std::string, Node*>;

} // namespace gc::detail
