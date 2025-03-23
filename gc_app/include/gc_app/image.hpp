/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_app/color.hpp"
#include "gc_app/types.hpp"

#include "common/struct_type_macro.hpp"


namespace gc_app {

template <typename T>
struct Size final
{
    T width;
    T height;

    auto operator==(const Size&) const noexcept -> bool = default;
};

using UintSize = Size<Uint>;

GCLIB_STRUCT_TYPE(UintSize, width, height);


using ColorVec =
    std::vector<Color>;

struct Image final
{
    UintSize size;
    ColorVec data;
};

} // namespace gc_app

GC_REGISTER_CUSTOM_TYPE(gc_app::Image, 1);
