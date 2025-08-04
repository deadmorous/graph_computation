#pragma once

#include "build/lib_config_fwd.hpp"

#include "common/struct_type_macro.hpp"

#include <ostream>
#include <string>
#include <vector>


namespace build {

struct LibConfig final
{
    std::vector<std::string> include_dirs;
    std::string binary_path;
    std::string compile_options;
};

GCLIB_STRUCT_TYPE(LibConfig, include_dirs, binary_path, compile_options);

auto operator<<(std::ostream&, const LibConfig&)
    -> std::ostream&;

} // namespace build
