#include "gc/type_registry.hpp"

#include "gc/type.hpp"
#include "gc/value_path.hpp"

namespace gc {

auto populate_gc_type_registry(gc::TypeRegistry& result)
    -> void
{
    result.register_value("ValuePath", type_of<ValuePath>());
}


} // namespace gc
