#include "lib_config/lib_config.hpp"

#include "common/throw.hpp"

#include <cassert>
#include <unordered_map>

namespace build {

namespace {

using LibConfigMap = std::unordered_map<std::string_view, LibConfig>;

auto lib_config_map()
    -> LibConfigMap&
{
    static auto result = LibConfigMap{};
    return result;
}

} // anonymous namespace

auto register_lib_config(std::string_view library, LibConfig lib_config)
    -> void
{
    auto& m = lib_config_map();
    if (m.contains(library))
        common::throw_(
            "register_lib_config failed: Library '",
            library,
            "' is already registered");
    m.emplace(library, std::move(lib_config));
}

auto lib_config(std::string_view library)
    -> LibConfig const&
{
    auto& m = lib_config_map();
    auto it = m.find(library);
    if (it == m.end())
        common::throw_(
            "lib_config failed: Library '",
            library,
            "' is not found");
    return it->second;
}

} // namespace build
