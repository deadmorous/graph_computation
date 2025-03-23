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

#include "gc/node_index.hpp"

#include <unordered_map>


namespace gc::detail {

template <typename Node>
using NodeIndices = std::unordered_map<const Node*, NodeIndex>;

} // namespace gc::detail
