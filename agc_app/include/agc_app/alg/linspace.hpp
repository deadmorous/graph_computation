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

#include "agc_app/types/linspace_spec.hpp"


namespace agc_app {

struct LinSpaceIter final
{
    double v;
    double h;
    size_t n;
};

auto LinSpaceInitIter(const LinSpaceSpec&)
    -> LinSpaceIter;

auto LinSpaceDerefIter(const LinSpaceIter&)
    -> double;

auto LinSpaceNextIter(LinSpaceIter&)
    -> bool;

} // namespace agc_app
