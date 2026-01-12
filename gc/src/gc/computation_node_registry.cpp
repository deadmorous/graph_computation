/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/computation_node_registry.hpp"

namespace gc {

template<>
auto populate_gc_node_registry(gc::ComputationNodeRegistry&)
    -> void
{
    // gc does not define any nodes at the moment
}

} // namespace gc
