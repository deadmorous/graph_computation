#include <gtest/gtest.h>

// -------- GENERATED CODE BEGIN --------

#include "agc_app/alg/canvas.hpp"
#include "agc_app/alg/counter.hpp"
#include "agc_app/alg/grid_2d.hpp"
#include "agc_app/alg/mag2.hpp"
#include "agc_app/alg/mandelbrot_func.hpp"
#include "agc_app/alg/scale.hpp"
#include "agc_app/alg/threshold.hpp"
#include "agc_app/types/canvas.hpp"
#include "agc_app/types/grid_2d_spec.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace { // INSERTED MANUALLY

// Libraries
// - agc_app


// Variable types

using T14 =
    agc_app::Grid2dSpec;

using T52 =
    uint64_t;

using T62 =
    std::array<double, 2>;

using T70 =
    std::array<double, 2>;

using T71 =
    std::array<double, 2>;

using T85 =
    std::array<double, 2>;

using T112 =
    uint64_t;

using T113 =
    uint64_t;

using T128 =
    std::array<double, 2>;

using T138 =
    double;

using T152 =
    uint64_t;

using T160 =
    uint64_t;

using T168 =
    double;

using T169 =
    uint64_t;

using T193 =
    agc_app::Canvas<double>;

using T194 =
    double;

using T195 =
    agc_app::CanvasSize;

using T196 =
    agc_app::CanvasPixel<double>;

using T197 =
    double;

using T21 =
    std::decay_t<decltype(agc_app::grid_2d_size(std::declval<T14>()))>;

using T18 =
    std::decay_t<decltype(agc_app::grid_2d_init_iter(std::declval<T14>()))>;

using T78 =
    std::decay_t<decltype(agc_app::mandelbrot_func(std::declval<T70>(), std::declval<T71>()))>;

using T104 =
    std::decay_t<decltype(agc_app::mandelbrot_func(std::declval<T70>(), std::declval<T71>()))>;

using T94 =
    std::decay_t<decltype(agc_app::mandelbrot_func(std::declval<T70>(), std::declval<T71>()))>;

using T84 =
    std::decay_t<decltype(agc_app::mandelbrot_func(std::declval<T70>(), std::declval<T71>()))>;

using T132 =
    std::decay_t<decltype(agc_app::mag2(std::declval<T128>()))>;

using T139 =
    std::decay_t<decltype(agc_app::mag2(std::declval<T128>()))>;

using T174 =
    std::decay_t<decltype(agc_app::scale(std::declval<T168>(), std::declval<T169>()))>;

using T23 =
    std::decay_t<decltype(agc_app::grid_2d_deref_iter(std::declval<T18>()))>;

using T37 =
    std::decay_t<decltype(agc_app::grid_2d_deref_iter(std::declval<T18>()))>;


// Context type
struct Context final
{
    // Node 0 (Grid2d)
    T14 var_14;

    // Node 1 (Split)
    T37 var_37;

    // Node 2 (Counter)
    T52 var_52;

    // Node 3 (Replicate)
    T62 var_62;

    // Node 4 (MandelbrotFunc)
    T70 var_70;
    T71 var_71;

    // Node 5 (FuncIterator)
    T85 var_85;
    T84 var_84;

    // Node 6 (Split)
    T94 var_94;

    // Node 7 (Replicate)
    T104 var_104;

    // Node 8 (Threshold)
    T112 var_112;
    T113 var_113;

    // Node 9 (Mag2)
    T128 var_128;

    // Node 10 (Threshold)
    T138 var_138;
    T139 var_139;

    // Node 11 (Split)
    T152 var_152;

    // Node 12 (Replicate)
    T160 var_160;

    // Node 13 (Scale)
    T168 var_168;
    T169 var_169;

    // Node 14 (Canvas)
    T195 var_195;
    T196 var_196;
    T197 var_197;
    T193 var_193;
    T194 var_194;
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

// Node 0 (Grid2d)

auto activate_node_0_in_0(Context& ctx)
    -> void
{
  auto var_21 = agc_app::grid_2d_size(ctx.var_14);
  auto var_18 = agc_app::grid_2d_init_iter(ctx.var_14);
  // Activate output port out_0
  // Activate [O(0,0)->I(14,0)]
  ctx.var_195 = var_21;
  activate_node_14_in_0(ctx);
  do
  {
    auto var_23 = agc_app::grid_2d_deref_iter(var_18);
    // Activate output port out_1
    // Activate [O(0,1)->I(1,0)]
    ctx.var_37 = var_23;
    activate_node_1_in_0(ctx);
  }
  while (agc_app::grid_2d_next_iter(var_18));
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
  ctx.var_70 = ctx.var_37;
  activate_node_4_in_0(ctx);
  // Activate output port out_3
  // Activate [O(1,3)->I(5,2)]
  activate_node_5_in_2(ctx);
}

// Node 2 (Counter)

auto activate_node_2_in_0(Context& ctx)
    -> void
{
  agc_app::next_counter(ctx.var_52);
  // Activate output port out_0
  // Activate [O(2,0)->I(11,0)]
  ctx.var_152 = ctx.var_52;
  activate_node_11_in_0(ctx);
}

auto activate_node_2_in_1(Context& ctx)
    -> void
{
  agc_app::reset_counter(ctx.var_52);
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
  ctx.var_85 = ctx.var_62;
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
  auto var_78 = agc_app::mandelbrot_func(ctx.var_70, ctx.var_71);
  // Activate output port out_0
  // Activate [O(4,0)->I(5,1)]
  ctx.var_84 = var_78;
  activate_node_5_in_1(ctx);
}

// Node 5 (FuncIterator)

auto activate_node_5_in_0(Context& ctx)
    -> void
{
  ctx.var_84 = ctx.var_85;
}

auto activate_node_5_in_1(Context& ctx)
    -> void
{
  // Activate output port out_1
  // Activate [O(5,1)->I(6,0)]
  ctx.var_94 = ctx.var_84;
  activate_node_6_in_0(ctx);
}

auto activate_node_5_in_2(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(5,0)->I(4,1)]
  ctx.var_71 = ctx.var_84;
  activate_node_4_in_1(ctx);
}

// Node 6 (Split)

auto activate_node_6_in_0(Context& ctx)
    -> void
{
  // Activate output port out_0
  // Activate [O(6,0)->I(7,0)]
  ctx.var_104 = ctx.var_94;
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
  ctx.var_128 = ctx.var_104;
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
  if (agc_app::threshold(ctx.var_113, ctx.var_112))
  {
    // Activate output port out_0
    // Activate [O(8,0)->I(7,1)]
    activate_node_7_in_1(ctx);
  }
  else
  {
    // Activate output port out_1
    // Activate [O(8,1)->I(13,1)]
    ctx.var_169 = ctx.var_113;
    activate_node_13_in_1(ctx);
  }
}

// Node 9 (Mag2)

auto activate_node_9_in_0(Context& ctx)
    -> void
{
  auto var_132 = agc_app::mag2(ctx.var_128);
  // Activate output port out_0
  // Activate [O(9,0)->I(10,1)]
  ctx.var_139 = var_132;
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
  if (agc_app::threshold(ctx.var_139, ctx.var_138))
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
  ctx.var_160 = ctx.var_152;
  activate_node_12_in_0(ctx);
  // Activate output port out_1
  // Activate [O(11,1)->I(8,1)]
  ctx.var_113 = ctx.var_152;
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
  ctx.var_169 = ctx.var_160;
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
  auto var_174 = agc_app::scale(ctx.var_168, ctx.var_169);
  // Activate output port out_0
  // Activate [O(13,0)->I(14,2)]
  ctx.var_197 = var_174;
  activate_node_14_in_2(ctx);
}

// Node 14 (Canvas)

auto activate_node_14_in_0(Context& ctx)
    -> void
{
  agc_app::resize_canvas(ctx.var_193, ctx.var_195);
}

auto activate_node_14_in_1(Context& ctx)
    -> void
{
  agc_app::set_canvas_pixel(ctx.var_193, ctx.var_196);
}

auto activate_node_14_in_2(Context& ctx)
    -> void
{
  agc_app::set_next_canvas_pixel(ctx.var_193, ctx.var_197);
}

auto activate_node_14_in_3(Context& ctx)
    -> void
{
  // Activate output port out_0
}

auto activate_node_14_in_4(Context& ctx)
    -> void
{
  agc_app::clear_canvas(ctx.var_193);
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
    using namespace agc_app;
    uint64_t iter_count = 100;
    auto ctx = Context {
        // Grid
        .var_14 = Grid2dSpec{
            .rect = {Range<double>{ -2.1, 0.7 },
                     Range<double>{ -1.2, 1.2 } },
            .resolution = { 0.1, 0.2 } },

        // z0 for iterations
        .var_62 = { 0., 0. },

        // Iteration count
        .var_112 = iter_count,

        // Mag2 threshold (to detect iterations divergence)
        .var_138 = 1000.,

        // Scale factor for final magnitude of iterated value
        .var_168 = 1. / iter_count
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

    EXPECT_EQ(common::format(ctx.var_193), expected);
}
