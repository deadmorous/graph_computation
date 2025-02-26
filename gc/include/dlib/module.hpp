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
