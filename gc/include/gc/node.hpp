#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value_fwd.hpp"

#include "common/const_name_span.hpp"
#include "common/func_ref_fwd.hpp"

#include <cassert>
#include <limits>
#include <stop_token>


namespace gc {

using NodeProgress =
    common::FuncRef<void(double)>;

struct Node
{
    virtual ~Node() = default;

    virtual auto input_names() const -> common::ConstNameSpan = 0;

    virtual auto output_names() const -> common::ConstNameSpan = 0;

    virtual auto default_inputs(ValueSpan result) const -> void = 0;

    virtual auto compute_outputs(ValueSpan result,
                                 ConstValueSpan inputs,
                                 const std::stop_token& stoken,
                                 const NodeProgress& progress) const
        -> bool = 0;

    auto input_count() const -> uint8_t
    {
        auto result = input_names().size();
        assert(result <= std::numeric_limits<uint8_t>::max());
        return result;
    }

    auto output_count() const -> uint8_t
    {
        auto result = output_names().size();
        assert(result <= std::numeric_limits<uint8_t>::max());
        return result;
    }

};

} // namespace gc
