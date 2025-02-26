#pragma once

#include "gc/type_registry.hpp"
#include "gc/value_fwd.hpp"

#include <yaml-cpp/node/node.h>


namespace gc::yaml {

auto parse_value(const YAML::Node& node,
                 const TypeRegistry& type_registry)
    -> Value;

auto parse_value(const YAML::Node& node,
                 const Type* type,
                 const TypeRegistry& type_registry)
    -> Value;

} // namespace gc::yaml
