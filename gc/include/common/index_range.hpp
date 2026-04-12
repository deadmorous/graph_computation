#pragma once
// Compatibility shim — use mpk/mix/util/index_range.hpp directly in new code.
#include "mpk/mix/util/index_range.hpp"

namespace common::detail
{
using mpk::mix::detail::IndexTraits;
template <mpk::mix::detail::IndexLikeType T>
using IndexIterator = mpk::mix::detail::IndexIterator<T>;
} // namespace common::detail

namespace common
{
using mpk::mix::IndexRange;
using mpk::mix::index_range;
using mpk::mix::sized_index_range;
} // namespace common
