#pragma once
// Compatibility shim — use mpk/mix/util/detail/index_like.hpp directly in new code.
#include "mpk/mix/util/detail/index_like.hpp"

namespace common::detail
{
using mpk::mix::detail::IndexLikeType;
template <mpk::mix::detail::IndexLikeType T>
using IndexDiffType = mpk::mix::detail::IndexDiffType<T>;
} // namespace common::detail
