#pragma once

#include "build/lib_config.hpp"

namespace build {

auto register_lib_config(std::string_view library, LibConfig lib_config)
    -> void;

auto lib_config(std::string_view library)
    -> LibConfig const&;

} // namespace build
