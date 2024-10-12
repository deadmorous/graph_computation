#include "gc_app/multiply.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;

namespace gc_app {

class Multiply final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    { return gc::node_input_names<Multiply>( "lhs"sv, "rhs"sv ); }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<Multiply>( "product"sv ); }


    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 2);
        result[0] = uint_val(2);
        result[1] = uint_val(3);
    }

    auto compute_outputs(
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
};

auto make_multiply(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("Multiply", args);
    return std::make_shared<Multiply>();
}

} // namespace gc_app
