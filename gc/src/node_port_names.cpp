/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/node_port_names.hpp"

#include "common/format.hpp"


namespace gc {

DynamicNames::DynamicNames(std::string_view prefix, WeakPort count)
    : prefix_{ prefix }
{ resize(count); }

auto DynamicNames::resize(WeakPort count)
    -> void
{
    if (names_.size() == count)
        return;

    if (names_.size() > count)
        names_.resize(count);

    else
        for (WeakPort i=names_.size(); i<count; ++i)
            names_.push_back(common::format(prefix_, int(i)));

    name_views_.clear();
    for (const auto& name : names_)
        name_views_.push_back(name);
}

auto DynamicNames::operator()() const
    -> common::ConstNameSpan
{
    return name_views_;
}

} // namespace gc
