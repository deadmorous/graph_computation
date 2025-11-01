/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/computation_node_registry.hpp"

#include "gc_app/cell_aut/life.hpp"
#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/filter_seq.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/rect_view.hpp"
#include "gc_app/spiral_view.hpp"
#include "gc_app/test_sequence.hpp"
#include "gc_app/uint_size.hpp"
#include "gc_app/waring.hpp"
#include "gc_app/waring_parallel.hpp"

namespace gc_app {

template<>
auto populate_node_registry(gc::ComputationNodeRegistry& result)
    -> void
{
#define GC_APP_REGISTER(name) \
    result.register_value(#name, gc_app::make_##name)

    result.register_value("life", gc_app::cell_aut::make_life);

    GC_APP_REGISTER(eratosthenes_sieve);
    GC_APP_REGISTER(filter_seq);
    GC_APP_REGISTER(multiply);
    GC_APP_REGISTER(project);
    GC_APP_REGISTER(rect_view);
    GC_APP_REGISTER(spiral_view);
    GC_APP_REGISTER(test_sequence);
    GC_APP_REGISTER(uint_size);
    GC_APP_REGISTER(waring);
    GC_APP_REGISTER(waring_parallel);

#undef GC_APP_REGISTER
}

} // namespace gc_app
