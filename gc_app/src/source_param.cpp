#include "gc_app/source_param.hpp"

namespace gc_app {

class SourceParam final :
    public gc::Node,
    public InputParameters
{
public:
    auto input_count() const
        -> uint32_t
    { return 0; }

    auto output_count() const
        -> uint32_t
    { return param_.size(); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {}

    auto compute_outputs(gc::ValueSpan result,
                                      gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.empty());
        get_inputs(result);
    }


    auto get_inputs(gc::ValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == param_.size());
        std::copy(param_.begin(), param_.end(), inputs.begin());
    }

    auto set_inputs(gc::ConstValueSpan inputs)
        -> void
    {
        param_.reserve(inputs.size());
        param_.clear();
        std::copy(inputs.begin(), inputs.end(), std::back_inserter(param_));
    }

private:
    gc::ValueVec param_;
};

auto make_source_param()
    -> std::shared_ptr<gc::Node>
{ return std::make_shared<SourceParam>(); }

} // namespace gc_app
