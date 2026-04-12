#pragma once
// Compatibility shim — use mpk/mix/strong/span.hpp directly in new code.
#include "common/index_range.hpp"
#include "mpk/mix/strong/span.hpp"

namespace common
{
using mpk::mix::StrongSpan;
} // namespace common
