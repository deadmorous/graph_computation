#pragma once

#include "gc/port.hpp"
#include "gc/value_fwd.hpp"

#include "common/strong_span.hpp"

#include <string_view>


namespace gc {

using InputNames =
    common::StrongSpan<const std::string_view, InputPort>;

using OutputNames =
    common::StrongSpan<const std::string_view, OutputPort>;

using OutputValues =
    common::StrongSpan<Value, OutputPort>;

using InputValues =
    common::StrongSpan<Value, InputPort>;

using ConstInputValues =
    common::StrongSpan<const Value, InputPort>;

} // namespace gc
