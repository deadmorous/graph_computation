/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

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
    std::string shared;
    std::string debug;
    std::string release;
    std::string gclib;
};

GCLIB_STRUCT_TYPE(CompileFlags, common, shared, debug, release, gclib);


struct LinkFlags final
{
    std::string common;
    std::string shared;
    std::string shared_unwrapped;
};

GCLIB_STRUCT_TYPE(LinkFlags, common, shared, shared_unwrapped);


struct FlagRules final
{
    std::string linker_wrapper_flag;
    std::string linker_wrapper_flag_sep;
    std::string include_flag;
    std::string include_system_flag;
};

GCLIB_STRUCT_TYPE(
    FlagRules,
    linker_wrapper_flag,
    linker_wrapper_flag_sep,
    include_flag,
    include_system_flag);


struct Config final
{
    Paths paths;
    CompileFlags compile_flags;
    LinkFlags link_flags;
    FlagRules flag_rules;
    std::string gclib_include_dirs;
};

GCLIB_STRUCT_TYPE(
    Config, paths, compile_flags, link_flags, flag_rules, gclib_include_dirs);


auto default_config()
    -> Config;

auto operator<<(std::ostream&, const Config&)
    -> std::ostream&;

} // namespace build
