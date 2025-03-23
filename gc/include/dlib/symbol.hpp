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

#include "dlib/symbol_fwd.hpp"

#include "common/impl_tag.hpp"
#include "common/type.hpp"


namespace dlib {

class Symbol final
{
public:
    Symbol() noexcept = default;

    Symbol(common::Impl_Tag, void* address) noexcept:
        address_{ address }
    {}

    auto empty() const noexcept
        -> bool
    { return address_ == nullptr; }

    template <typename T>
    auto as(common::Type_Tag<T> = {}) const noexcept
        -> T*
    { return { reinterpret_cast<T*>(address_) }; }

private:
    void* address_{};
};

} // namespace dlib
