/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/context_fwd.hpp"

#include "gc/node_registry.hpp"
#include "gc/type_registry.hpp"


namespace gc {

template <typename Node>
struct Context final
{
    TypeRegistry type_registry;
    NodeRegistry<Node> node_registry;
};

} // namespace gc
