// Compatibility shim — use mpk/mix/util/format_seq.hpp directly in new code.
#pragma once

#include "mpk/mix/util/format_seq.hpp"

namespace common
{
using mpk::mix::DefaultFormatter;
using mpk::mix::format_seq;
} // namespace common
