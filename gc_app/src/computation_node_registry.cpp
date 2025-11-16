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

#include "gc_app/nodes/cell_aut/cell2d.hpp"
#include "gc_app/nodes/cell_aut/gen_rule_reader.hpp"
#include "gc_app/nodes/cell_aut/generate_rules.hpp"
#include "gc_app/nodes/cell_aut/life.hpp"
#include "gc_app/nodes/cell_aut/offset_image.hpp"
#include "gc_app/nodes/cell_aut/random_image.hpp"
#include "gc_app/nodes/cell_aut/rule_reader.hpp"
#include "gc_app/nodes/num/filter_seq.hpp"
#include "gc_app/nodes/num/multiply.hpp"
#include "gc_app/nodes/num/eratosthenes_sieve.hpp"
#include "gc_app/nodes/num/test_sequence.hpp"
#include "gc_app/nodes/num/waring.hpp"
#include "gc_app/nodes/num/waring_parallel.hpp"
#include "gc_app/nodes/util/merge.hpp"
#include "gc_app/nodes/util/project.hpp"
#include "gc_app/nodes/util/uint_size.hpp"
#include "gc_app/nodes/visual/image_colorizer.hpp"
#include "gc_app/nodes/visual/image_loader.hpp"
#include "gc_app/nodes/visual/rect_view.hpp"
#include "gc_app/nodes/visual/spiral_view.hpp"

namespace gc_app {

template<>
auto populate_node_registry(gc::ComputationNodeRegistry& result)
    -> void
{
#define GC_APP_REGISTER(ns, name) \
    result.register_value(#name, gc_app::ns::make_##name)

    GC_APP_REGISTER(cell_aut, cell2d);
    GC_APP_REGISTER(cell_aut, gen_rule_reader);
    GC_APP_REGISTER(cell_aut, generate_rules);
    GC_APP_REGISTER(cell_aut, life);
    GC_APP_REGISTER(cell_aut, offset_image);
    GC_APP_REGISTER(cell_aut, random_image);
    GC_APP_REGISTER(cell_aut, rule_reader);
    GC_APP_REGISTER(num, eratosthenes_sieve);
    GC_APP_REGISTER(num, filter_seq);
    GC_APP_REGISTER(num, multiply);
    GC_APP_REGISTER(num, test_sequence);
    GC_APP_REGISTER(num, waring);
    GC_APP_REGISTER(num, waring_parallel);
    GC_APP_REGISTER(util, merge);
    GC_APP_REGISTER(util, project);
    GC_APP_REGISTER(util, uint_size);
    GC_APP_REGISTER(visual, image_colorizer);
    GC_APP_REGISTER(visual, image_loader);
    GC_APP_REGISTER(visual, rect_view);
    GC_APP_REGISTER(visual, spiral_view);

#undef GC_APP_REGISTER
}

} // namespace gc_app
