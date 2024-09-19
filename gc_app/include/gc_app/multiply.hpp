#pragma once

#include "gc/graph_computation.hpp"

namespace gc_app {

class Multiply final :
    gc::Node
{
public:
    auto input_count() const -> uint32_t override;

    auto output_count() const -> uint32_t override;

    auto default_inputs(gc::ValueSpan result) const -> void override;

    auto compute_outputs(gc::ValueSpan result,
                         gc::ConstValueSpan inputs) const -> void override;
};

} // namespace gc_app
