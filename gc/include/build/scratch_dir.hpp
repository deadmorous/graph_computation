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

#include "build/scratch_dir_fwd.hpp"

#include <filesystem>


namespace build {

class ScratchDir final
{
public:
    explicit ScratchDir(std::string_view name_prefix = {});

    explicit ScratchDir(const std::filesystem::path& parent_dir,
                        std::string_view name_prefix = {});

    ScratchDir(const ScratchDir&) = delete;
    ScratchDir(ScratchDir&&) = default;
    auto operator=(const ScratchDir&) -> ScratchDir& = delete;
    auto operator=(ScratchDir&&) -> ScratchDir& = default;

    ~ScratchDir();

    auto empty() const noexcept
        -> bool;

    auto detach() noexcept
        -> void;

    auto path() const noexcept
        -> const std::filesystem::path&;

    operator const std::filesystem::path&() const noexcept;

private:
    std::filesystem::path path_;
};

} // namespace build
