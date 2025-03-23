/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/alg/linspace.hpp"

#include <cassert>


namespace agc_app {

auto LinSpaceInitIter(const LinSpaceSpec& spec)
    -> LinSpaceIter
{
    assert(spec.count > 1);
    return {
        .v = spec.first,
        .h = (spec.last - spec.first) / (spec.count - 1),
        .n = spec.count
    };
}

auto LinSpaceDerefIter(const LinSpaceIter& it)
    -> double
{
    assert(it.n != 0);
    return it.v;
}

auto LinSpaceNextIter(LinSpaceIter& it)
    -> bool
{
    if (it.n <= 1) [[unlikely]]
        return false;

    --it.n;
    it.v += it.h;
    return true;
}

} // namespace agc_app
