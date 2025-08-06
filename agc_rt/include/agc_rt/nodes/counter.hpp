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

#include <cstdint>


namespace agc_rt {

inline auto reset_counter(uint64_t& counter) noexcept
    -> void
{ counter = 0; }

inline auto next_counter(uint64_t& counter) noexcept
    -> void
{ ++counter; }

} // namespace agc_rt
