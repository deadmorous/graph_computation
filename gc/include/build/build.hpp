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
#include "build/lib_config_fwd.hpp"
#include "build/output_type.hpp"
#include "build/scratch_dir_fwd.hpp"

#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>


namespace build {

using Inputs = std::span<const std::filesystem::path>;

using InputVec = std::vector<std::filesystem::path>;

struct BuildOptions final
{
    OutputType output_type = OutputType::Executable;
};

using LibConfigVec = std::vector<LibConfig>;

using Libs = std::span<const LibConfig>;

auto build(const Config&,
           const std::filesystem::path& output,
           const ScratchDir&,
           Inputs,
           Libs = {},
           const BuildOptions& = {})
    -> void;

} // namespace build
