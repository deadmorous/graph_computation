#pragma once

#include <span>

namespace gc {

class Value;

using ValueSpan = std::span<Value>;
using ConstValueSpan = std::span<const Value>;

} // namespace gc
