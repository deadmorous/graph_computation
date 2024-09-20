#pragma once

#include "gc/graph_computation.hpp"

namespace gc_app {

struct InputParameters
{
    virtual ~InputParameters() = default;

    virtual auto get_inputs(gc::ValueSpan inputs) const -> void = 0;

    virtual auto set_inputs(gc::ConstValueSpan inputs) -> void = 0;
};

class SourceParam final :
    public gc::Node,
    public InputParameters
{
public:
    auto input_count() const -> uint32_t override;

    auto output_count() const -> uint32_t override;

    auto default_inputs(gc::ValueSpan result) const -> void override;

    auto compute_outputs(gc::ValueSpan result,
                         gc::ConstValueSpan inputs) const -> void override;


    auto get_inputs(gc::ValueSpan inputs) const -> void override;

    auto set_inputs(gc::ConstValueSpan inputs) -> void override;

private:
    gc::ValueVec param_;
};

} // namespace gc_app
