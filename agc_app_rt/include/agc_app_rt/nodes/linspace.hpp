#pragma once

#include "agc_app_rt/types/linspace_spec.hpp"


namespace agc_app_rt {

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

} // namespace agc_app_rt
