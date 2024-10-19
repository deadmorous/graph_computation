#include "gc_app/node_registry.hpp"

#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/rect_view.hpp"
#include "gc_app/spiral_view.hpp"
#include "gc_app/test_sequence.hpp"

namespace gc_app {

auto populate_node_registry(gc::NodeRegistry& result)
    -> void
{
#define GC_APP_REGISTER(name) \
    result.register_value(#name, gc_app::make_##name)

    GC_APP_REGISTER(eratosthenes_sieve);
    GC_APP_REGISTER(multiply);
    GC_APP_REGISTER(project);
    GC_APP_REGISTER(rect_view);
    GC_APP_REGISTER(spiral_view);
    GC_APP_REGISTER(test_sequence);

#undef GC_APP_REGISTER
}

} // namespace gc_app
