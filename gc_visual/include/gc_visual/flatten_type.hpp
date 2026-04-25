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

#include "mpk/mix/value/type_fwd.hpp"
#include "mpk/mix/value/value_path.hpp"

#include <vector>


namespace gc_visual {

struct TypeComponent final
{
    const mpk::mix::value::Type* type;
    mpk::mix::value::ValuePath path;
};

using TypeComponentVec = std::vector<TypeComponent>;

auto flatten_type(const mpk::mix::value::Type* t)
    -> std::vector<TypeComponent>;

} // namespace gc_visual
