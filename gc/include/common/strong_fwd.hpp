#pragma once
// Compatibility shim — use mpk/mix/strong/fwd.hpp directly in new code.
#include "mpk/mix/strong/fwd.hpp"

namespace common
{
using mpk::mix::Strong;
using mpk::mix::is_strong_v;
using mpk::mix::StrongType;
using mpk::mix::StrongArithmeticType;
using mpk::mix::StrongNumericType;
using mpk::mix::StrongCountType;
using mpk::mix::StrongIndexType;
using mpk::mix::StrongStringType;
using mpk::mix::HasViewType;
using mpk::mix::StrongView;
} // namespace common
