#pragma once

#include "gc/node.hpp"
#include "gc/value_fwd.hpp"

#include "common/maybe_const.hpp"


namespace gc_app {

struct InputParameters
{
    virtual ~InputParameters() = default;

    virtual auto get_inputs(gc::ValueSpan inputs) const -> void = 0;

    virtual auto set_inputs(gc::ConstValueSpan inputs) -> void = 0;

    template <common::MaybeConst<gc::Node> Node>
    static auto get(Node* node)
        -> InputParameters*
    { return dynamic_cast<common::AsConstAs<Node, InputParameters*>>(node); }
};

auto make_source_param(gc::ConstValueSpan)
    -> std::shared_ptr<gc::Node>;

} // namespace gc_app
