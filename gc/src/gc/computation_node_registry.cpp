#include "gc/computation_node_registry.hpp"

namespace gc {

template<>
auto populate_gc_node_registry(gc::ComputationNodeRegistry& result)
    -> void
{
    // gc does not define any nodes at the moment
}

} // namespace gc
