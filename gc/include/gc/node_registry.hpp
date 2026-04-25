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

#include "gc/context_fwd.hpp"
#include "mpk/mix/value/value_fwd.hpp"

#include "mpk/mix/value/object_registry.hpp"
#include "mpk/mix/meta/type.hpp"


namespace gc {

template <typename Node>
using NodeRegistry =
    mpk::mix::value::ObjectRegistry<Node, mpk::mix::value::ConstValueSpan, const Context<Node>&>;

template <typename Node>
auto populate_gc_node_registry(NodeRegistry<Node>&)
    -> void;

template <typename Node>
auto node_registry(mpk::mix::Type_Tag<Node> = {})
    -> NodeRegistry<Node>
{
    auto result = NodeRegistry<Node>{};
    populate_gc_node_registry(result);
    return result;
}

} // namespace gc
