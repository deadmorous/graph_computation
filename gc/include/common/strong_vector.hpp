#pragma once
// Compatibility shim — use mpk/mix/strong/vector.hpp directly in new code.
#include "common/index_range.hpp"
#include "mpk/mix/strong/vector.hpp"

namespace common
{
using mpk::mix::StrongVector;
} // namespace common
