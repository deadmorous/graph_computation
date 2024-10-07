#pragma once

#include "gc/node_fwd.hpp"
#include "gc/value.hpp"

#include "common/const_name_span.hpp"

#include <memory>


namespace gc {

struct Node
{
    virtual ~Node() = default;

    virtual auto input_names() const -> common::ConstNameSpan = 0;

    virtual auto output_names() const -> common::ConstNameSpan = 0;

    virtual auto default_inputs(ValueSpan result) const -> void = 0;

    virtual auto compute_outputs(ValueSpan result,
                                 ConstValueSpan inputs) const -> void = 0;

    auto input_count() const -> uint32_t
    { return input_names().size(); }

    auto output_count() const -> uint32_t
    { return output_names().size(); }

};

using NodePtr = std::shared_ptr<Node>;

} // namespace gc
