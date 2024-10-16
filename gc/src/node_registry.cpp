#include "gc/node_registry.hpp"

#include "gc/source_param.hpp"

namespace gc {

auto populate_gc_node_registry(gc::NodeRegistry& result)
    -> void
{
    result.register_value("source_param", gc::make_source_param);
}

} // namespace gc
