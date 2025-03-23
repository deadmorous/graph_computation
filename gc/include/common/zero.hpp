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

constexpr inline struct Zero_Tag final {} Zero;

template <std::default_initializable T>
auto operator==(Zero_Tag, const T& x) noexcept -> bool
{ return x == T{}; }

template <std::default_initializable T>
auto operator==(const T& x, Zero_Tag) noexcept -> bool
{ return x == T{}; }

} // namespace common
