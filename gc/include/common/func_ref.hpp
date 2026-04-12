#pragma once
// Compatibility shim — use mpk/mix/func_ref/func_ref.hpp directly in new code.
// Re-include old dependencies for transitive-include compatibility.
#include "common/const.hpp"
#include "common/signature.hpp"
#include "common/unsafe.hpp"
#include "mpk/mix/func_ref/func_ref.hpp"

namespace common
{
using mpk::mix::FuncRef;
using mpk::mix::Indirect;
using mpk::mix::Indirect_Tag;
} // namespace common
