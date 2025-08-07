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


namespace agc_app_rt {

template <typename Value, typename Threshold>
auto threshold(Value v, Threshold t) noexcept
    -> bool
{ return v < t; }

} // namespace agc_app_rt
