#pragma once

namespace common {

template <auto value_>
struct Const_Tag final
{
    using type = decltype(value_);
    static constexpr auto value = value_;
};

template <auto value_>
constexpr inline auto Const = Const_Tag<value_>{};

} // namespace common
