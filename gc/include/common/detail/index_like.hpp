#pragma once

#include "common/strong_fwd.hpp"

#include <concepts>
#include <utility>


namespace common::detail {

template <typename T>
concept IndexLikeType =
    std::integral<T> || StrongIndexType<T>;

template <IndexLikeType T>
using IndexDiffType = decltype(std::declval<T>() - std::declval<T>());

} // namespace common::detail
