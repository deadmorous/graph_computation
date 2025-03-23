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


namespace common {

template <typename... Ts>
struct TypePack_Tag final
{
    static constexpr auto size = sizeof...(Ts);
};

template <typename... Ts>
constexpr inline auto TypePack = TypePack_Tag<Ts...>{};

} // namespace common
