/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once


namespace common {

template <typename T>
struct Type_Tag final
{
    using type = T;
};

template <typename T>
constexpr inline auto Type = Type_Tag<T>{};

} // namespace common
