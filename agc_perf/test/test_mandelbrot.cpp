/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_perf/canvas.hpp"
#include "agc_perf/mandelbrot.hpp"
#include "agc_perf/mandelbrot_param.hpp"

#include "common/format.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>


using namespace std::string_view_literals;

namespace agc_perf {
namespace {

TEST(AgcPerf_Mandelbrot, ExpectedResult)
{
    auto param = MandelbrotParam{};
    param.resolution[0] *= 10;
    param.resolution[1] *= 10;

    auto expected = R"(@@@@@@@@@@%%%%%%%%%%%%%%%%%@
@@@@@@@%%%%%%%%%%%%%%+%%%%%%
@@@@%%%%%%%%%%%%%%%  #%%%%%%
@@%%%%%%%%%%%%%% *     %##%%
%%%%%%%%%%%%%%%          %%%
%%%%%%%%%#   #           *%%
%*: :.                  %%%%
%%%%%%%%%#   #           *%%
%%%%%%%%%%%%%%%          %%%
@@%%%%%%%%%%%%%% *     %##%%
@@@@%%%%%%%%%%%%%%%  #%%%%%%
@@@@@@@%%%%%%%%%%%%%%+%%%%%%
)"sv;

    auto actual = common::format(mandelbrot_set(MandelbrotRefImpl, param));
    EXPECT_EQ(common::format(actual), expected);
}

TEST(AgcPerf_Mandelbrot, SameResult)
{
    auto param = MandelbrotParam{};
    // param.resolution[0] *= 10;
    // param.resolution[1] *= 10;

    auto c0 = mandelbrot_set(MandelbrotRefImpl, param);

    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGraphImpl, param));
    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGraphImpl2, param));
    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGraphImpl3, param));
    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGraphImpl4, param));
    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGraphImpl5, param));
    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGraphImpl6, param));
    EXPECT_EQ(c0, mandelbrot_set(MandelbrotFineGotoGraphImpl, param));
}

} // anonymous namespace
} // namespace agc_perf

