#pragma once
// Compatibility shim — use mpk/mix/util/const_name_span.hpp directly in new code.
#include "common/type.hpp"
#include "common/type_pack.hpp"
#include "mpk/mix/util/const_name_span.hpp"

namespace common
{
using mpk::mix::ConstNameSpan;
using mpk::mix::const_name_span;
} // namespace common
