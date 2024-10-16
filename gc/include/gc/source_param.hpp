#pragma once

#include "gc/node.hpp"
#include "gc/value_fwd.hpp"

#include "common/maybe_const.hpp"


namespace gc {

struct InputParameters
{
    virtual ~InputParameters() = default;

    virtual auto get_inputs(ValueSpan inputs) const -> void = 0;

    virtual auto set_inputs(ConstValueSpan inputs) -> void = 0;

    template <common::MaybeConst<Node> Node>
    static auto get(Node* node)
        -> common::AsConstAs<Node, InputParameters>*
    { return dynamic_cast<common::AsConstAs<Node, InputParameters>*>(node); }
};

auto make_source_param(ConstValueSpan)
    -> std::shared_ptr<Node>;

} // namespace gc
