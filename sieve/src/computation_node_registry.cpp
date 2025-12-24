/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "sieve/computation_node_registry.hpp"

#include "sieve/nodes/i8_image_metrics.hpp"

namespace sieve {

template<>
auto populate_node_registry(gc::ComputationNodeRegistry& result)
    -> void
{
#define SIEVE_REGISTER(name) \
    result.register_value(#name, sieve::make_##name)

    SIEVE_REGISTER(i8_image_metrics);

#undef SIEVE_REGISTER
}

} // namespace sieve
