#pragma once

#include "gc/computation_node_fwd.hpp"
#include "gc/port_values.hpp"

#include "common/const_name_span.hpp"
#include "common/func_ref_fwd.hpp"

#include <cassert>
#include <limits>
#include <stop_token>


namespace gc {

using NodeProgress =
    common::FuncRef<void(double)>;

struct ComputationNode
{
    virtual ~ComputationNode() = default;

    virtual auto input_names() const -> InputNames = 0;

    virtual auto output_names() const -> OutputNames = 0;

    virtual auto default_inputs(InputValues result) const -> void = 0;

    virtual auto compute_outputs(OutputValues result,
                                 ConstInputValues inputs,
                                 const std::stop_token& stoken,
                                 const NodeProgress& progress) const
        -> bool = 0;

    auto input_count() const -> InputPortCount
    { return input_names().size(); }

    auto output_count() const -> OutputPortCount
    { return output_names().size(); }

};

} // namespace gc
