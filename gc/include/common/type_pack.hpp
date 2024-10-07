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
