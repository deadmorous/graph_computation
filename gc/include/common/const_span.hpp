#pragma once
// Compatibility shim — use mpk/mix/util/const_span.hpp directly in new code.
#include "common/type.hpp"
#include "common/type_pack.hpp"
#include "mpk/mix/util/const_span.hpp"

namespace common
{
using mpk::mix::const_span;
} // namespace common
