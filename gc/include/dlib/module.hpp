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

#include "dlib/module_fwd.hpp"

#include "dlib/symbol_fwd.hpp"
#include "dlib/symbol_name.hpp"

#include <filesystem>
#include <memory>


namespace dlib {

class Module final
{
public:
    Module() noexcept;

    explicit Module(const std::filesystem::path&);

    ~Module();

    auto empty() const noexcept
        -> bool;

    auto symbol(SymbolName::View) const
        -> Symbol;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace dlib
