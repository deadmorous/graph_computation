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

#include "common/struct_type_macro.hpp"


namespace agc_app {

template <typename T>
struct Range
{
    double begin;
    double end;

    constexpr auto length() const noexcept -> double
    { return end - begin; }
};

GCLIB_STRUCT_TYPE(Range<double>, begin, end);
GCLIB_STRUCT_TYPE(Range<float>, begin, end);

} // namespace agc_app
