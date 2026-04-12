#pragma once
// Compatibility shim — use mpk/mix/util/tuple_like.hpp directly in new code.
#include "mpk/mix/util/tuple_like.hpp"

namespace common
{
using mpk::mix::is_tuple_like_v;
using mpk::mix::tuple_like;
} // namespace common
