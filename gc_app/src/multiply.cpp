#include "gc_app/multiply.hpp"

#include "gc_app/types.hpp"


namespace gc_app {

auto Multiply::input_count() const
    -> uint32_t
{ return 2; }

auto Multiply::output_count() const
    -> uint32_t
{ return 1; }

auto Multiply::default_inputs(gc::ValueSpan result) const
    -> void
{
    assert(result.size() == 2);
    result[0] = uint_val(2);
    result[1] = uint_val(3);
}

auto Multiply::compute_outputs(
        gc::ValueSpan result,
        gc::ConstValueSpan inputs) const
    -> void
{
    assert(inputs.size() == 2);
    assert(result.size() == 1);
    auto type = inputs[0].type();
    assert(type == inputs[1].type());
    assert(type->aggregate_type() == gc::AggregateType::Scalar);
    gc::ScalarT(type).visit_numeric(
        [&](auto tag)
        {
            result[0] = inputs[0].as(tag) * inputs[1].as(tag);
        });
}

} // namespace gc_app
