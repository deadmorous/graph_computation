/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "sieve/types/i8_range.hpp"

#include "common/throw.hpp"

#include <limits>

namespace sieve {

I8Range::I8Range(int min_state, int state_count):
    min_state_{min_state},
    state_count_{state_count}
{
    enforce_invariants();
}

auto I8Range::length() const noexcept ->  int
{
    return state_count_;
}

auto I8Range::first() const noexcept -> int
{
    return min_state_;
}

auto I8Range::enforce_invariants() -> void
{
    constexpr int min = std::numeric_limits<int8_t>::min();
    constexpr int max = std::numeric_limits<int8_t>::max();

    auto end = min_state_ + state_count_;
    if (min_state_ < min || state_count_ < 0 || end > max+1)
        common::throw_<std::range_error>(
            "Range [", min_state_, ", ", end, ") is invalid");
}

} // namespace sieve
