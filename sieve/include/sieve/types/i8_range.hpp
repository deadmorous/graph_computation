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

namespace sieve {

class I8Range final
{
public:
    constexpr I8Range() = default;

    I8Range(int min_state, int state_count);

    auto length() const noexcept ->  int;
    auto first() const noexcept -> int;

private:
    auto enforce_invariants() -> void;

    int min_state_{};
    int state_count_{};
};

} // namespace sieve
