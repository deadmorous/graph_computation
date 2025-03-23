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


namespace agc_perf {

struct Range
{
    double begin;
    double end;

    constexpr auto length() const noexcept -> double
    { return end - begin; }
};

} // namespace agc_perf
