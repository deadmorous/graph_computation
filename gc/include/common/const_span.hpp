#pragma once

#include "common/type.hpp"
#include "common/type_pack.hpp"

#include <concepts>
#include <span>

namespace common {


template <typename... Ids, typename T, std::same_as<T>... Args>
auto const_span(TypePack_Tag<Ids...>, Type_Tag<T>,  Args... values)
    -> std::span<const T>
{
    static const std::array<T, sizeof...(Args)> result = { values... };
    return result;
}

} // namespace common
