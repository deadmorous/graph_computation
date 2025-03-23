/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "common/type.hpp"


namespace common {

template <typename T>
concept StructType =
    requires(T t, const T ct)
{
    fields_of(t);
    fields_of(ct);
    tuple_tag_of(Type<T>);
    field_names_of(Type<T>);
};

} // namespace common
