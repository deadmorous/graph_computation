/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <type_traits>


namespace agc_app {

template <typename Factor, typename Value>
auto scale(Factor f, Value v) noexcept
    -> std::common_type_t<Factor, Value>
{ return f * v; }

} // namespace agc_app
