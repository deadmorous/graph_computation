#pragma once

#include "gc/computation_node_fwd.hpp"
#include "gc/port.hpp"
#include "gc/value_fwd.hpp"

#include "common/const_name_span.hpp"
#include "common/func_ref_fwd.hpp"
#include "common/strong_span.hpp"

#include <cassert>
#include <limits>
#include <stop_token>


namespace gc {

using NodeProgress =
    common::FuncRef<void(double)>;

using OutputValues =
    common::StrongSpan<Value, OutputPort>;

using InputValues =
    common::StrongSpan<Value, InputPort>;

using ConstInputValues =
    common::StrongSpan<const Value, InputPort>;

struct ComputationNode
{
    virtual ~ComputationNode() = default;

    virtual auto input_names() const -> common::ConstNameSpan = 0;

    virtual auto output_names() const -> common::ConstNameSpan = 0;

    virtual auto default_inputs(InputValues result) const -> void = 0;

    virtual auto compute_outputs(OutputValues result,
                                 ConstInputValues inputs,
                                 const std::stop_token& stoken,
                                 const NodeProgress& progress) const
        -> bool = 0;

    auto input_count() const -> InputPortCount
    {
        auto result = input_names().size();
        assert(result <= std::numeric_limits<WeakPort>::max());
        return InputPortCount(result);
    }

    auto output_count() const -> OutputPortCount
    {
        auto result = output_names().size();
        assert(result <= std::numeric_limits<WeakPort>::max());
        return OutputPortCount(result);
    }

};

} // namespace gc
