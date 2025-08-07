/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include <gtest/gtest.h>
// -------- GENERATED CODE BEGIN --------

#include "agc_app_rt/nodes/canvas.hpp"
#include "agc_app_rt/nodes/counter.hpp"
#include "agc_app_rt/nodes/grid_2d.hpp"
#include "agc_app_rt/nodes/mag2.hpp"
#include "agc_app_rt/nodes/mandelbrot_func.hpp"
#include "agc_app_rt/nodes/printer.hpp"
#include "agc_app_rt/nodes/scale.hpp"
#include "agc_app_rt/nodes/threshold.hpp"
#include "agc_app_rt/types/canvas.hpp"
#include "agc_app_rt/types/grid_2d_spec.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace { // INSERTED MANUALLY

// Libraries
// - agc_app_rt


// Variable types

using T14 =
    agc_app_rt::Grid2dSpec;

using T54 =
    uint64_t;

using T64 =
    std::array<double, 2>;

using T72 =
    std::array<double, 2>;

using T73 =
    std::array<double, 2>;

using T87 =
    std::array<double, 2>;

using T114 =
    uint64_t;

using T115 =
    uint64_t;

using T130 =
    std::array<double, 2>;

using T140 =
    double;

using T154 =
    uint64_t;

using T162 =
    uint64_t;

using T170 =
    double;

using T171 =
    uint64_t;

using T195 =
    agc_app_rt::Canvas<double>;

using T196 =
    double;

using T197 =
    agc_app_rt::CanvasSize;

using T198 =
    agc_app_rt::CanvasPixel<double>;

using T199 =
    double;

using T220 =
    agc_app_rt::Canvas<double>;

using T21 =
    std::decay_t<decltype(agc_app_rt::grid_2d_size(std::declval<T14>()))>;

using T18 =
    std::decay_t<decltype(agc_app_rt::grid_2d_init_iter(std::declval<T14>()))>;

using T80 =
    std::decay_t<decltype(agc_app_rt::mandelbrot_func(std::declval<T72>(), std::declval<T73>()))>;

using T106 =
    std::decay_t<decltype(agc_app_rt::mandelbrot_func(std::declval<T72>(), std::declval<T73>()))>;

using T96 =
    std::decay_t<decltype(agc_app_rt::mandelbrot_func(std::declval<T72>(), std::declval<T73>()))>;

using T86 =
    std::decay_t<decltype(agc_app_rt::mandelbrot_func(std::declval<T72>(), std::declval<T73>()))>;

using T134 =
    std::decay_t<decltype(agc_app_rt::mag2(std::declval<T130>()))>;

using T141 =
    std::decay_t<decltype(agc_app_rt::mag2(std::declval<T130>()))>;

using T176 =
    std::decay_t<decltype(agc_app_rt::scale(std::declval<T170>(), std::declval<T171>()))>;

using T23 =
    std::decay_t<decltype(agc_app_rt::grid_2d_deref_iter(std::declval<T18>()))>;

using T39 =
    std::decay_t<decltype(agc_app_rt::grid_2d_deref_iter(std::declval<T18>()))>;


// Context type
struct Context final
{
    // Node 0 (Grid2d)
    T14 var_14;

    // Node 1 (Split)
    T39 var_39;

    // Node 2 (Counter)
    T54 var_54;

    // Node 3 (Replicate)
    T64 var_64;

    // Node 4 (MandelbrotFunc)
    T72 var_72;
    T73 var_73;

    // Node 5 (FuncIterator)
    T87 var_87;
    T86 var_86;

    // Node 6 (Split)
    T96 var_96;

    // Node 7 (Replicate)
    T106 var_106;

    // Node 8 (Threshold)
    T114 var_114;
    T115 var_115;

    // Node 9 (Mag2)
    T130 var_130;

    // Node 10 (Threshold)
    T140 var_140;
    T141 var_141;

    // Node 11 (Split)
    T154 var_154;

    // Node 12 (Replicate)
    T162 var_162;

    // Node 13 (Scale)
    T170 var_170;
    T171 var_171;

    // Node 14 (Canvas)
    T197 var_197;
    T198 var_198;
    T199 var_199;
    T195 var_195;
    T196 var_196;

    // Node 15 (Printer)
    T220 var_220;
};


// Node algorithms

auto activate_node_0_in_0(Context& ctx)
    -> void;
auto activate_node_1_in_0(Context& ctx)
    -> void;
auto activate_node_2_in_0(Context& ctx)
    -> void;
auto activate_node_2_in_1(Context& ctx)
    -> void;
auto activate_node_3_in_0(Context& ctx)
    -> void;
auto activate_node_3_in_1(Context& ctx)
    -> void;
auto activate_node_4_in_0(Context& ctx)
    -> void;
auto activate_node_4_in_1(Context& ctx)
    -> void;
auto activate_node_5_in_0(Context& ctx)
    -> void;
auto activate_node_5_in_1(Context& ctx)
    -> void;
auto activate_node_5_in_2(Context& ctx)
    -> void;
auto activate_node_6_in_0(Context& ctx)
    -> void;
auto activate_node_7_in_0(Context& ctx)
    -> void;
auto activate_node_7_in_1(Context& ctx)
    -> void;
auto activate_node_8_in_0(Context& ctx)
    -> void;
auto activate_node_8_in_1(Context& ctx)
    -> void;
auto activate_node_9_in_0(Context& ctx)
    -> void;
auto activate_node_10_in_0(Context& ctx)
    -> void;
auto activate_node_10_in_1(Context& ctx)
    -> void;
auto activate_node_11_in_0(Context& ctx)
    -> void;
auto activate_node_12_in_0(Context& ctx)
    -> void;
auto activate_node_12_in_1(Context& ctx)
    -> void;
auto activate_node_13_in_0(Context& ctx)
    -> void;
auto activate_node_13_in_1(Context& ctx)
    -> void;
auto activate_node_14_in_0(Context& ctx)
    -> void;
auto activate_node_14_in_1(Context& ctx)
    -> void;
auto activate_node_14_in_2(Context& ctx)
    -> void;
auto activate_node_14_in_3(Context& ctx)
    -> void;
auto activate_node_14_in_4(Context& ctx)
    -> void;
auto activate_node_15_in_0(Context& ctx)
    -> void;

// Node 0 (Grid2d)

auto activate_node_0_in_0(Context& ctx)
    -> void
{
  auto var_21 = agc_app_rt::grid_2d_size(ctx.var_14);
  auto var_18 = agc_app_rt::grid_2d_init_iter(ctx.var_14);
  // Activate output port out_0
  // Activate [O(0,0)->I(14,0)]
  ctx.var_197 = var_21;
  activate_node_14_in_0(ctx);
  do
  {
    auto var_23 = agc_app_rt::grid_2d_deref_iter(var_18);
    // Activate output port out_1
    // Activate [O(0,1)->I(1,0)]
    ctx.var_39 = var_23;
    activate_node_1_in_0(ctx);
  }
  while (agc_app_rt::grid_2d_next_iter(var_18));
  // Activate output port out_2
  // Activate [O(0,2)->I(14,3)]
  activate_node_14_in_3(ctx);
}

// Node 1 (Split)

auto activate_node_1_in_0(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(1,0)->I(2,1)]
  activate_node_2_in_1(ctx);
  // Activate output port out_1
  // Activate [O(1,1)->I(3,1)]
  activate_node_3_in_1(ctx);
  // Activate output port out_2
  // Activate [O(1,2)->I(4,0)]
  ctx.var_72 = ctx.var_39;
  activate_node_4_in_0(ctx);
  // Activate output port out_3
  // Activate [O(1,3)->I(5,2)]
  activate_node_5_in_2(ctx);
}

// Node 2 (Counter)

auto activate_node_2_in_0(Context& ctx)
    -> void
{
  agc_app_rt::next_counter(ctx.var_54);
  // Activate output port out_0
  // Activate [O(2,0)->I(11,0)]
  ctx.var_154 = ctx.var_54;
  activate_node_11_in_0(ctx);
}

auto activate_node_2_in_1(Context& ctx)
    -> void
{
  agc_app_rt::reset_counter(ctx.var_54);
}

// Node 3 (Replicate)

auto activate_node_3_in_0(Context& ctx)
    -> void
{
}

auto activate_node_3_in_1(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(3,0)->I(5,0)]
  ctx.var_87 = ctx.var_64;
  activate_node_5_in_0(ctx);
}

// Node 4 (MandelbrotFunc)

auto activate_node_4_in_0(Context& ctx)
    -> void
{
}

auto activate_node_4_in_1(Context& ctx)
    -> void
{
  auto var_80 = agc_app_rt::mandelbrot_func(ctx.var_72, ctx.var_73);
  // Activate output port out_0
  // Activate [O(4,0)->I(5,1)]
  ctx.var_86 = var_80;
  activate_node_5_in_1(ctx);
}

// Node 5 (FuncIterator)

auto activate_node_5_in_0(Context& ctx)
    -> void
{
  ctx.var_86 = ctx.var_87;
}

auto activate_node_5_in_1(Context& ctx)
    -> void
{
  // Activate output port out_1
  // Activate [O(5,1)->I(6,0)]
  ctx.var_96 = ctx.var_86;
  activate_node_6_in_0(ctx);
}

auto activate_node_5_in_2(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(5,0)->I(4,1)]
  ctx.var_73 = ctx.var_86;
  activate_node_4_in_1(ctx);
}

// Node 6 (Split)

auto activate_node_6_in_0(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(6,0)->I(7,0)]
  ctx.var_106 = ctx.var_96;
  activate_node_7_in_0(ctx);
  // Activate output port out_1
  // Activate [O(6,1)->I(2,0)]
  activate_node_2_in_0(ctx);
  // Activate output port out_2
}

// Node 7 (Replicate)

auto activate_node_7_in_0(Context& ctx)
    -> void
{
}

auto activate_node_7_in_1(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(7,0)->I(9,0)]
  ctx.var_130 = ctx.var_106;
  activate_node_9_in_0(ctx);
}

// Node 8 (Threshold)

auto activate_node_8_in_0(Context& ctx)
    -> void
{
}

auto activate_node_8_in_1(Context& ctx)
    -> void
{
  if (agc_app_rt::threshold(ctx.var_115, ctx.var_114))
  {
    // Activate output port out_0
    // Activate [O(8,0)->I(7,1)]
    activate_node_7_in_1(ctx);
  }
  else
  {
    // Activate output port out_1
    // Activate [O(8,1)->I(13,1)]
    ctx.var_171 = ctx.var_115;
    activate_node_13_in_1(ctx);
  }
}

// Node 9 (Mag2)

auto activate_node_9_in_0(Context& ctx)
    -> void
{
  auto var_134 = agc_app_rt::mag2(ctx.var_130);
  // Activate output port out_0
  // Activate [O(9,0)->I(10,1)]
  ctx.var_141 = var_134;
  activate_node_10_in_1(ctx);
}

// Node 10 (Threshold)

auto activate_node_10_in_0(Context& ctx)
    -> void
{
}

auto activate_node_10_in_1(Context& ctx)
    -> void
{
  if (agc_app_rt::threshold(ctx.var_141, ctx.var_140))
  {
    // Activate output port out_0
    // Activate [O(10,0)->I(5,2)]
    activate_node_5_in_2(ctx);
  }
  else
  {
    // Activate output port out_1
    // Activate [O(10,1)->I(12,1)]
    activate_node_12_in_1(ctx);
  }
}

// Node 11 (Split)

auto activate_node_11_in_0(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(11,0)->I(12,0)]
  ctx.var_162 = ctx.var_154;
  activate_node_12_in_0(ctx);
  // Activate output port out_1
  // Activate [O(11,1)->I(8,1)]
  ctx.var_115 = ctx.var_154;
  activate_node_8_in_1(ctx);
}

// Node 12 (Replicate)

auto activate_node_12_in_0(Context& ctx)
    -> void
{
}

auto activate_node_12_in_1(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(12,0)->I(13,1)]
  ctx.var_171 = ctx.var_162;
  activate_node_13_in_1(ctx);
}

// Node 13 (Scale)

auto activate_node_13_in_0(Context& ctx)
    -> void
{
}

auto activate_node_13_in_1(Context& ctx)
    -> void
{
  auto var_176 = agc_app_rt::scale(ctx.var_170, ctx.var_171);
  // Activate output port out_0
  // Activate [O(13,0)->I(14,2)]
  ctx.var_199 = var_176;
  activate_node_14_in_2(ctx);
}

// Node 14 (Canvas)

auto activate_node_14_in_0(Context& ctx)
    -> void
{
  agc_app_rt::resize_canvas(ctx.var_195, ctx.var_197);
}

auto activate_node_14_in_1(Context& ctx)
    -> void
{
  agc_app_rt::set_canvas_pixel(ctx.var_195, ctx.var_198);
}

auto activate_node_14_in_2(Context& ctx)
    -> void
{
  agc_app_rt::set_next_canvas_pixel(ctx.var_195, ctx.var_199);
}

auto activate_node_14_in_3(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(14,0)->I(15,0)]
  ctx.var_220 = ctx.var_195;
  activate_node_15_in_0(ctx);
}

auto activate_node_14_in_4(Context& ctx)
    -> void
{
  agc_app_rt::clear_canvas(ctx.var_195);
}

// Node 15 (Printer)

auto activate_node_15_in_0(Context& ctx)
    -> void
{
  agc_app_rt::print(ctx.var_220);
}

auto entry_point( Context& ctx )
    -> void
{
  activate_node_10_in_0(ctx);
  activate_node_8_in_0(ctx);
  activate_node_3_in_0(ctx);
  activate_node_13_in_0(ctx);
  activate_node_0_in_0(ctx);
}

} // anonymous namespace - INSERTED MANUALLY

// -------- GENERATED CODE END --------


TEST(AgcApp_GenTest, Mandelbrot)
{
    using namespace agc_app_rt;
    uint64_t iter_count = 100;
    auto ctx = Context {
        // Grid
        .var_14 = Grid2dSpec{
            .rect = {Range<double>{ -2.1, 0.7 },
                     Range<double>{ -1.2, 1.2 } },
            .resolution = { 0.1, 0.2 } },

        // z0 for iterations
        .var_64 = { 0., 0. },

        // Iteration count
        .var_114 = iter_count,

        // Mag2 threshold (to detect iterations divergence)
        .var_140 = 1000.,

        // Scale factor for final magnitude of iterated value
        .var_170 = 1. / iter_count
    };

    entry_point(ctx);

    using namespace std::string_view_literals;

    constexpr auto expected = R"(@@@@@@@@@@@@@@@@@@%%%%@@@@@@
@@@@@@@@@@@@@@%%%%%%%+%%%@@@
@@@@@@@@@@@%%%%%%%%  #%%%%%@
@@@@@@@%%%%%%%%% *     %##%%
@@@@@%%%%%%%%%%          %%%
@@%%%%%%%#   #           *%%
@*- :.                  %%%%
@@%%%%%%%#   #           *%%
@@@@@%%%%%%%%%%          %%%
@@@@@@@%%%%%%%%% *     %##%%
@@@@@@@@@@@%%%%%%%%  #%%%%%@
@@@@@@@@@@@@@@%%%%%%%+%%%@@@
)"sv;

    EXPECT_EQ(common::format(ctx.var_195), expected);
}
