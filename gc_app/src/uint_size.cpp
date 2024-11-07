#include "gc_app/uint_size.hpp"

#include "gc_app/image.hpp"
#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"

using namespace std::string_view_literals;

namespace gc_app {

class UintSizeNode final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<UintSizeNode>( "width"sv, "height"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<UintSizeNode>( "size"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    {
        assert(result.size() == 2);
        result[0] = uint_val(320);
        result[1] = uint_val(200);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 2);
        assert(result.size() == 1);
        auto width = uint_val(inputs[0]);
        auto height = uint_val(inputs[1]);
        result[0] = UintSize{ width, height };
        return true;
    }
};

auto make_uint_size(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("UintSizeNode", args);
    return std::make_shared<UintSizeNode>();
}

} // namespace gc_app
