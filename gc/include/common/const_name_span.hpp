#pragma once

#include "common/const_span.hpp"

#include <string_view>

namespace common {

using ConstNameSpan =
    std::span<const std::string_view>;

template <typename... Ids, std::same_as<std::string_view>... Args>
auto const_name_span(TypePack_Tag<Ids...> ids, Args... values)
    -> ConstNameSpan
{ return const_span(ids, Type<std::string_view>, values...); }

} // namespace common
