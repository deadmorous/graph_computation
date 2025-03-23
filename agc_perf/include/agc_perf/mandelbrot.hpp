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

#include "agc_perf/canvas_fwd.hpp"
#include "agc_perf/mandelbrot_param_fwd.hpp"

#include <string_view>


#define AGC_BM_DECL_MANDELBROT_SET_IMPL(type, label)                        \
    constexpr inline struct type##_Tag final {} type;                       \
                                                                            \
    template <>                                                             \
    constexpr inline std::string_view tag_label<type##_Tag> = label;        \
                                                                            \
    auto mandelbrot_set(type##_Tag, const MandelbrotParam& param)           \
        -> Canvas


namespace agc_perf {

template <typename T>
constexpr inline std::string_view tag_label;

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotRefImpl,
    "reference implementation");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGraphImpl,
    "fine graph implementation");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGraphImpl2,
    "fine graph implementation 2");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGraphImpl3,
    "fine graph implementation 3");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGraphImpl4,
    "fine graph implementation 4");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGraphImpl5,
    "fine graph implementation 5");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGraphImpl6,
    "fine graph implementation 6");

AGC_BM_DECL_MANDELBROT_SET_IMPL(
    MandelbrotFineGotoGraphImpl,
    "fine GOTO graph implementation");

} // namespace agc_perf

#undef AGC_BM_DECL_MANDELBROT_SET_IMPL
