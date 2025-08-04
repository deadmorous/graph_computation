#include "build/lib_config.hpp"

#include "gc/value.hpp"


namespace build {

auto operator<<(std::ostream& s, const LibConfig& lib_config)
    -> std::ostream&
{ return s << gc::Value{ lib_config }; }

} // namespace build
