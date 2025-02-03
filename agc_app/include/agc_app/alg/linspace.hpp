#pragma once

#include "agc_app/types/linspace_spec.hpp"


namespace agc_app {

struct LinSpaceIter
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
