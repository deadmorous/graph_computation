#include "gc/source_param.hpp"

#include "gc/node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include "common/throw.hpp"

namespace gc {

class SourceParam final :
    public Node,
    public InputParameters
{
public:
    explicit SourceParam(ConstValueSpan param) :
        param_( param.begin(), param.end() ),
        out_names_(param.size())
    {}

    auto input_names() const
        -> common::ConstNameSpan override
    { return {}; }

    auto output_names() const
        -> common::ConstNameSpan override
    { return out_names_(); }

    auto default_inputs(ValueSpan result) const
        -> void override
    {}

    auto compute_outputs(ValueSpan result,
                         ConstValueSpan inputs,
                         const std::stop_token& stoken,
                         const NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.empty());
        get_inputs(result);
        return true;
    }


    auto get_inputs(ValueSpan inputs) const
        -> void override
    {
        assert(inputs.size() == param_.size());
        std::copy(param_.begin(), param_.end(), inputs.begin());
    }

    auto set_inputs(ConstValueSpan inputs)
        -> void override
    {
        if (inputs.size() != param_.size())
            common::throw_<std::invalid_argument>(
                "SourceParam: incorrect number of inputs: expected ",
                param_.size(), ", got ", inputs.size());
        param_.clear();
        out_names_.resize(inputs.size());
        std::copy(inputs.begin(), inputs.end(), std::back_inserter(param_));
    }

private:
    ValueVec param_;
    DynamicOutputNames out_names_;
};

auto make_source_param(ConstValueSpan args)
    -> std::shared_ptr<Node>
{
    return std::make_shared<SourceParam>(args);
}

} // namespace gc
