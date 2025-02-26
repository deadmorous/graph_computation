#pragma once

#include "build/config_fwd.hpp"

#include "common/struct_type_macro.hpp"

#include <ostream>
#include <string>


namespace build {

struct Paths final
{
    std::string cxx_compiler_executable;
    std::string linker_executable;
};

GCLIB_STRUCT_TYPE(Paths, cxx_compiler_executable, linker_executable);


struct CompileFlags final
{
    std::string common;
    std::string debug;
    std::string release;
};

GCLIB_STRUCT_TYPE(CompileFlags, common, debug, release);


struct LinkFlags final
{
    std::string common;
    std::string shared;
};

GCLIB_STRUCT_TYPE(LinkFlags, common, shared);


struct Config final
{
    Paths paths;
    CompileFlags compile_flags;
    LinkFlags link_flags;
};

GCLIB_STRUCT_TYPE(Config, paths, compile_flags, link_flags);


auto default_config()
    -> Config;

auto operator<<(std::ostream&, const Config&)
    -> std::ostream&;

} // namespace build
