#include "build/config.hpp"

#include "gc/value.hpp"


namespace build {

auto operator<<(std::ostream& s, const Config& config)
    -> std::ostream&
{ return s << gc::Value{ config }; }

} // namespace build
