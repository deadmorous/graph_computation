#include "build/config.hpp"

#include "gc/value.hpp"
#include "gc/yaml/parse_value.hpp"


namespace build {

auto from_yaml(const YAML::Node& node)
    -> Config
{
    const auto* type = gc::Type::of<Config>();
    auto value = gc::yaml::parse_value(node, type, {});
    return value.as<Config>();
}

auto operator<<(std::ostream& s, const Config& config)
    -> std::ostream&
{ return s << gc::Value{ config }; }

} // namespace build
