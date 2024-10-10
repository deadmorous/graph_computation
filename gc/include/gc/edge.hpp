#pragma once

#include "gc/node_fwd.hpp"

#include <array>
#include <cstdint>


namespace gc {

struct EdgeEnd final
{
    const Node*             node;
    uint32_t                port;
};

using Edge = std::array<EdgeEnd, 2>;

} // namespace gc
