#include "agc_app/activation_node_registry.hpp"

#include "agc_app/nodes/canvas.hpp"
#include "agc_app/nodes/counter.hpp"
#include "agc_app/nodes/func_iterator.hpp"
#include "agc_app/nodes/grid_2d.hpp"
#include "agc_app/nodes/linspace.hpp"
#include "agc_app/nodes/mag2.hpp"
#include "agc_app/nodes/mandelbrot_func.hpp"
#include "agc_app/nodes/printer.hpp"
#include "agc_app/nodes/replicate.hpp"
#include "agc_app/nodes/scale.hpp"
#include "agc_app/nodes/split.hpp"
#include "agc_app/nodes/threshold.hpp"

namespace agc_app {

template<>
auto populate_node_registry(gc::ActivationNodeRegistry& result)
    -> void
{
#define GC_APP_REGISTER(name) \
    result.register_value(#name, agc_app::make_##name)

    GC_APP_REGISTER(canvas);
    GC_APP_REGISTER(counter);
    GC_APP_REGISTER(func_iterator);
    GC_APP_REGISTER(grid_2d);
    GC_APP_REGISTER(linspace);
    GC_APP_REGISTER(mag2);
    GC_APP_REGISTER(mandelbrot_func);
    GC_APP_REGISTER(printer);
    GC_APP_REGISTER(replicate);
    GC_APP_REGISTER(scale);
    GC_APP_REGISTER(split);
    GC_APP_REGISTER(threshold);

#undef GC_APP_REGISTER
}

} // namespace agc_app
