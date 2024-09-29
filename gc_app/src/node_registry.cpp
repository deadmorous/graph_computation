#include "gc_app/node_registry.hpp"

#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/rect_view.hpp"
#include "gc_app/source_param.hpp"
#include "gc_app/test_sequence.hpp"

namespace gc_app {

auto node_registry()
    -> NodeRegistry
{
    auto result = NodeRegistry{};

#define GC_APP_REGISTER(name) \
    result.register_factory(#name, gc_app::make_##name)

    GC_APP_REGISTER(eratosthenes_sieve);
    GC_APP_REGISTER(multiply);
    GC_APP_REGISTER(project);
    GC_APP_REGISTER(rect_view);
    GC_APP_REGISTER(source_param);
    GC_APP_REGISTER(test_sequence);

#undef GC_APP_REGISTER

    return result;
}

} // namespace gc_app
