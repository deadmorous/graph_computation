#include "gc_app/source_param.hpp"

#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


namespace gc_app {

class SourceParam final :
    public gc::Node,
    public InputParameters
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    { return {}; }

    auto output_names() const
        -> common::ConstNameSpan
    { return out_names_(); }

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
        out_names_.resize(inputs.size());
        std::copy(inputs.begin(), inputs.end(), std::back_inserter(param_));
    }

private:
    gc::ValueVec param_;
    gc::DynamicOutputNames out_names_;
};

auto make_source_param()
    -> std::shared_ptr<gc::Node>
{ return std::make_shared<SourceParam>(); }

} // namespace gc_app
