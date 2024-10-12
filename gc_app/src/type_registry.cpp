#include "gc_app/type_registry.hpp"

#include "gc_app/image.hpp"

namespace gc_app {

auto populate_type_registry(gc::TypeRegistry& result)
    -> void
{
    result.register_value("UintSize", gc::type_of<UintSize>());
}

} // namespace gc_app
