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

#include <string>
#include <unordered_map>


namespace gc::detail {

template <typename Node>
using NamedNodes = std::unordered_map<std::string, Node*>;

} // namespace gc::detail
