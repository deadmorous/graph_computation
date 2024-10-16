#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_path.hpp"

namespace gc {

struct ParameterSpec
{
    gc::Node*       node;
    size_t          index;
    gc::ValuePath   path;
};

} // namespace gc
