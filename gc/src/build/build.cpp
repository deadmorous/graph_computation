#include "build/build.hpp"

#include "build/config.hpp"
#include "build/config_vars.hpp"
#include "build/lib_config.hpp"
#include "build/scratch_dir.hpp"

#include "common/format.hpp"
#include "common/throw.hpp"

#include "sys/wait.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

#define GCLIB_LOG_BUILD_COMMANDS


namespace build {

namespace {

auto checked_system(const std::string& cmdln)
    -> void
{
#ifdef GCLIB_LOG_BUILD_COMMANDS
    std::cout << "EXECUTING: " << cmdln << std::endl;
#endif // GCLIB_LOG_BUILD_COMMANDS

    auto status = system(cmdln.c_str());
    if (WIFEXITED(status))
    {
        auto exit_status = WEXITSTATUS(status);
        if (exit_status == 0)
            return;
        common::throw_(
            "The following command exited with status ", exit_status, ":\n",
            cmdln);
    }

    if (WIFSIGNALED(status))
        common::throw_(
            "The following command was killed with signal ", WTERMSIG(status),
            ":\n", cmdln);

    if (WIFSIGNALED(status))
        common::throw_(
            "The following command was stopped with signal ", WSTOPSIG(status),
            ":\n", cmdln);

    common::throw_(
        "Unexpected status ", status, " for the following command:\n", cmdln);
}

} // anonymous namespace


auto build(const Config& config,
           const std::filesystem::path& output,
           const ScratchDir& scratch_dir,
           Inputs inputs,
           Libs libs,
           const BuildOptions& options)
    -> void
{
    const auto& scratch = scratch_dir.path();
    auto obj_outputs = std::vector<std::filesystem::path>{};

    auto append_flag = [](std::string& flags, const std::string& flag)
    {
        if (flag.empty())
            return;
        if (!flags.empty())
            flags += ' ';
        flags += flag;
    };

    auto append_link_flag = [&](std::string& flags, const std::string& flag)
    {
        if (flag.empty())
            return;

        const auto& wflag = config.flag_rules.linker_wrapper_flag;
        return append_flag(flags, wflag + flag);
    };

    auto compile_flags = config.compile_flags.common;
    append_flag(
        compile_flags,
        std::string{
            current_config_type == ConfigType::Debug
                ? config.compile_flags.debug
                : config.compile_flags.release });

    for (auto const& lib : libs)
        for (auto const& include_dir : lib.include_dirs)
            append_flag(compile_flags, "-I " + include_dir);

    if (options.output_type == OutputType::SharedObject)
        append_flag(compile_flags, config.compile_flags.shared);

    for (const auto& input : inputs)
    {
        const auto& output =
            obj_outputs.emplace_back(
                scratch / (input.filename().string() + ".o"));
        auto compile_cmdln = common::format(
            config.paths.cxx_compiler_executable,
            ' ', compile_flags,
            " -c -o ", output, ' ', input);
        checked_system(compile_cmdln);
    }

    auto link_flags = std::string{};
    append_link_flag(link_flags, config.link_flags.common);
    if (options.output_type == OutputType::SharedObject)
    {
        append_flag(link_flags, config.link_flags.shared_unwrapped);
        append_link_flag(link_flags, config.link_flags.shared);
    }

    if (!libs.empty())
    {
        if (options.output_type == OutputType::SharedObject)
            append_link_flag(link_flags, "--whole-archive");
        for (auto const& lib : libs)
            append_link_flag(link_flags, lib.binary_path);
        if (options.output_type == OutputType::SharedObject)
            append_link_flag(link_flags, "--no-whole-archive");
    }

    auto link_cmdln = common::format(
        config.paths.cxx_compiler_executable,
        ' ', config.compile_flags.shared,
        ' ', link_flags,
        " -o ", output,
        ' ', common::format_seq(obj_outputs, " "));
    checked_system(link_cmdln);
}

} // namespace build
