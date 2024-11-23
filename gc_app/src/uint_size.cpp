#include "gc_app/uint_size.hpp"

#include "gc_app/image.hpp"
#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"

using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app {

class UintSizeNode final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<UintSizeNode>( "width"sv, "height"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<UintSizeNode>( "size"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = uint_val(320);
        result[1_gc_i] = uint_val(200);
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 2_gc_ic);
        assert(result.size() == 1_gc_oc);
        auto width = uint_val(inputs[0_gc_i]);
        auto height = uint_val(inputs[1_gc_i]);
        result.front() = UintSize{ width, height };
        return true;
    }
};

auto make_uint_size(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("UintSizeNode", args);
    return std::make_shared<UintSizeNode>();
}

} // namespace gc_app
