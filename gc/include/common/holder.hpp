#pragma once
// Compatibility shim — use mpk/mix/util/holder.hpp directly in new code.
#include "mpk/mix/util/holder.hpp"

namespace common
{
using mpk::mix::ValueHolder;
using mpk::mix::ReferenceHolder;
using mpk::mix::BaseOf;
using mpk::mix::HolderType;
} // namespace common
