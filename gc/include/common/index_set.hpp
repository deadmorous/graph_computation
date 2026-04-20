#pragma once
// Compatibility shim — use mpk/mix/util/index_set.hpp directly in new code.
#include "common/strong.hpp"
#include "common/index_range.hpp"
#include "mpk/mix/util/index_set.hpp"

namespace common
{
using mpk::mix::IndexSet;
} // namespace common
