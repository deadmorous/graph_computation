#include "lib_config/lib_config.hpp"

#include "mpk/mix/util/throw.hpp"

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
        mpk::mix::throw_(
            "register_lib_config failed: Library '{}' is already registered",
            library);
    m.emplace(library, std::move(lib_config));
}

auto lib_config(std::string_view library)
    -> LibConfig const&
{
    auto& m = lib_config_map();
    auto it = m.find(library);
    if (it == m.end())
        mpk::mix::throw_(
            "lib_config failed: Library '{}' is not found", library);
    return it->second;
}

} // namespace build
