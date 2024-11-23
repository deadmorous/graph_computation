#pragma once

#include "gc/port.hpp"
#include "gc/value_fwd.hpp"

#include "common/strong_span.hpp"

namespace gc {

using OutputValues =
    common::StrongSpan<Value, OutputPort>;

using InputValues =
    common::StrongSpan<Value, InputPort>;

using ConstInputValues =
    common::StrongSpan<const Value, InputPort>;

} // namespace gc
