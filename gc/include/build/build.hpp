#pragma once

#include "build/config_fwd.hpp"

#include <filesystem>
#include <span>
#include <vector>


namespace build {

using Inputs = std::span<const std::filesystem::path>;

using InputVec = std::vector<std::filesystem::path>;

auto build(const Config&,
           const std::filesystem::path& output,
           Inputs)
    -> void;

} // namespace build
