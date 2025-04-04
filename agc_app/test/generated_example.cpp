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

#include "agc_app/alg/linspace.hpp"
#include "agc_app/alg/printer.hpp"
#include "agc_app/types/linspace_spec.hpp"

// Libraries
// - agc_app


// Variable types

using T8 =
    agc_app::LinSpaceSpec;

using T12 =
    std::decay_t<decltype(agc_app::LinSpaceInitIter(std::declval<T8>()))>;

using T15 =
    std::decay_t<decltype(agc_app::LinSpaceDerefIter(std::declval<T12>()))>;

using T28 =
    std::decay_t<decltype(agc_app::LinSpaceDerefIter(std::declval<T12>()))>;


// Context type
struct Context final
{
    // Node 0 (LinSpace)
    T8 var_8;

    // Node 1 (Printer)
    T28 var_28;
};


// Node algorithms

auto activate_node_0_in_0(Context& ctx)
    -> void;
auto activate_node_1_in_0(Context& ctx)
    -> void;

// Node 0 (LinSpace)

auto activate_node_0_in_0(Context& ctx)
    -> void
{
  auto var_12 = agc_app::LinSpaceInitIter(ctx.var_8);
  do
  {
    auto var_15 = agc_app::LinSpaceDerefIter(var_12);
    // Activate [O(0,0)->I(1,0)]
    ctx.var_28 = var_15;
    activate_node_1_in_0(ctx);
  }
  while (agc_app::LinSpaceNextIter(var_12));
}

// Node 1 (Printer)

auto activate_node_1_in_0(Context& ctx)
    -> void
{
  agc_app::print(ctx.var_28);
}

// -------- GENERATED CODE END --------

TEST(AgcApp_GenTest, Hello)
{
    auto ctx = Context {
        .var_8 = agc_app::LinSpaceSpec{
            .first = 10,
            .last = 20,
            .count = 21
        }
    };

    activate_node_0_in_0(ctx);
}
