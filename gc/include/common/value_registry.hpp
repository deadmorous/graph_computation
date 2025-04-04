/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/throw.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <optional>
#include <string_view>


namespace common {

template <typename T>
class ValueRegistry
{
public:
    using Storage =
        std::map<std::string_view, T>;

    using StorageConstIterator =
        typename Storage::const_iterator;

    auto register_value(std::string_view name, T value)
        -> void
    {
        assert(!storage_.contains(name));
        storage_.emplace(name, value);
    }

    auto maybe_at(std::string_view name) const noexcept
        -> std::optional<T>
    {
        auto it = storage_.find(name);
        if (it == storage_.end())
            return std::nullopt;
        return it->second;
    }

    auto at(std::string_view name) const
        -> T
    {
        auto result = maybe_at(name);
        if (result.has_value())
            return *result;
        throw_<std::domain_error>(
            "No value is registered for name '", name, "'");
    }

    auto size() const noexcept
    { return storage_.size(); }

    auto begin() const noexcept
        -> StorageConstIterator
    { return storage_.begin(); }

    auto end() const noexcept
        -> StorageConstIterator
    { return storage_.end(); }

private:
    Storage storage_;
};

} // namespace common
