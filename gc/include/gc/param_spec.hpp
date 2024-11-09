#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_path.hpp"

namespace gc {

struct ParameterSpec
{
    size_t          input;
    gc::ValuePath   path;
};

} // namespace gc
