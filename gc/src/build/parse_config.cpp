#include "build/parse_config.hpp"

#include "build/config.hpp"
#include "build/lib_config.hpp"

#include "gc/value.hpp"
#include "gc/yaml/parse_value.hpp"


namespace build {

namespace {

template <typename T>
auto parse(const YAML::Node& node)
    -> T
{
    const auto* type = gc::Type::of<T>();
    auto value = gc::yaml::parse_value(node, type, {});
    return value.template as<T>();
}

} // anonymous namespace

auto parse_config(const YAML::Node& node)
    -> Config
{ return parse<Config>(node); }

auto parse_lib_config(const YAML::Node& node)
    -> LibConfig
{ return parse<LibConfig>(node); }

} // namespace build
