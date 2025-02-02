#if 0
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
  while (agc_app::LinSpaceNextIter(var_12))
  {
    auto var_15 = agc_app::LinSpaceDerefIter(var_12);
    // Activate [O(0,0)->I(1,0)]
    ctx.var_28 = var_15;
    activate_node_1_in_0(ctx);
  }
}

// Node 1 (Printer)

auto activate_node_1_in_0(Context& ctx)
    -> void
{
  agc_app::print(ctx.var_28);
}

#endif // 0
