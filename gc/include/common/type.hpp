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
