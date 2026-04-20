#pragma once
// Compatibility shim — use mpk/mix/util/checkpoint.hpp directly in new code.
#include "common/linked_list.hpp"
#include "mpk/mix/util/checkpoint.hpp"

namespace common
{
using mpk::mix::UpdateHistoryType;
using mpk::mix::Checkpoint;
} // namespace common
