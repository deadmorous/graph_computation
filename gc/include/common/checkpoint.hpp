/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "common/linked_list.hpp"

#include <concepts>
#include <functional>
#include <utility>
#include <type_traits>

namespace common {

template <typename T>
concept UpdateHistoryType = requires(T update_history)
{
    requires std::is_default_constructible_v<T>;
    requires std::is_copy_constructible_v<T>;
    update_history.reset(); // no history available or no previous checkpoint
    update_history.clear(); // no changes since last checkpoint
};

template <UpdateHistoryType UpdateHistory>
class Checkpoint final :
    public common::IntrusiveLinkedListConnectivity<Checkpoint<UpdateHistory>>
{
public:
    template<std::invocable<UpdateHistory&> F>
    auto update(F&& f) -> void
    {
        std::invoke(std::forward<F>(f), update_history_);
    }

    auto sync() -> UpdateHistory
    {
        auto result = update_history_;
        update_history_.clear();
        return result;
    }

private:
    UpdateHistory update_history_;
};

} // namespace common
