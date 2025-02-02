#pragma once

#include "agc_app/types/linspace_spec.hpp"


namespace agc_app {

struct LinSpaceIter
{};

auto LinSpaceInitIter(const LinSpaceSpec&)
    -> LinSpaceIter;

auto LinSpaceDerefIter(const LinSpaceIter&)
    -> double;

auto LinSpaceNextIter(LinSpaceIter&)
    -> bool;

} // namespace agc_app
