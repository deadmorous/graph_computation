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

#include <concepts>

namespace common {

template <typename T, typename... Ts>
concept AnyOf = (std::same_as<T, Ts> || ...);

} // namespace common
