#include "gc_app/source_param.hpp"

namespace gc_app {

auto SourceParam::input_count() const
    -> uint32_t
{ return 0; }

auto SourceParam::output_count() const
    -> uint32_t
{ return param_.size(); }

auto SourceParam::default_inputs(gc::ValueSpan result) const
    -> void
{}

auto SourceParam::compute_outputs(gc::ValueSpan result,
                                  gc::ConstValueSpan inputs) const
    -> void
{
    assert(inputs.empty());
    get_inputs(result);
}


auto SourceParam::get_inputs(gc::ValueSpan inputs) const
    -> void
{
    assert(inputs.size() == param_.size());
    std::copy(param_.begin(), param_.end(), inputs.begin());
}

auto SourceParam::set_inputs(gc::ConstValueSpan inputs)
    -> void
{
    param_.reserve(inputs.size());
    param_.clear();
    std::copy(inputs.begin(), inputs.end(), std::back_inserter(param_));
}

} // namespace gc_app
