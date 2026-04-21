#pragma once
// Compatibility shim — use mpk/mix/util/detail/hash.hpp directly in new code.
#include "mpk/mix/util/detail/hash.hpp"

namespace common::detail
{
using mpk::mix::detail::hash;
using mpk::mix::detail::Hash;
} // namespace common::detail
