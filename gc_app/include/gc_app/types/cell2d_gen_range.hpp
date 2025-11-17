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


namespace gc_app {

struct Cell2dIndexRange final
{
    int min{};
    int max{};
    int step{1};

    auto operator==(const Cell2dIndexRange&) const noexcept
        -> bool = default;

    bool ok(int global_min, int global_max) const noexcept
    {
        auto definition_ok =
            (min < max && step > 0) || (min == max && step >= 0);
        auto range_ok =
            min >= global_min && max <= global_max;
        return definition_ok && range_ok;
    }
};

GCLIB_STRUCT_TYPE(
    Cell2dIndexRange, min, max, step);

} // namespace gc_app
