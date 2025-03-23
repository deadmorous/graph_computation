/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/node_registry.hpp"


namespace gc_app {

template <typename Node>
auto populate_node_registry(gc::NodeRegistry<Node>&)
    -> void;

template <typename Node>
auto node_registry(common::Type_Tag<Node> = {})
    -> gc::NodeRegistry<Node>
{
    auto result = gc::NodeRegistry<Node>{};
    populate_node_registry(result);
    return result;
}

} // namespace gc_app
