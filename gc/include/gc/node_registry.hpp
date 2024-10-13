#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_fwd.hpp"

#include "common/object_registry.hpp"

namespace gc {

using NodeRegistry =
    common::ObjectRegstry<Node, ConstValueSpan>;

} // namespace gc