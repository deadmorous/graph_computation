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


namespace common {

template <typename T>
class ScopedInc
{
public:
    explicit ScopedInc(T& value) : value_{value}
    {
        ++value_;
    }

    ~ScopedInc()
    {
        --value_;
    }

    ScopedInc(const ScopedInc&) = delete;
    auto operator=(const ScopedInc&) -> ScopedInc& = delete;

private:
    T& value_;
};

} // namespace common
