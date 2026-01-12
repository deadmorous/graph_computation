/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/nodes/num/multiply.hpp"

#include "gc_app/types/uint.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app::num {

class Multiply final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Multiply>( "lhs"sv, "rhs"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Multiply>( "product"sv ); }


    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = uint_val(2);
        result[1_gc_i] = uint_val(3);
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress&) const
        -> bool override
    {
        assert(inputs.size() == 2_gc_ic);
        assert(result.size() == 1_gc_oc);
        auto type = inputs[0_gc_i].type();
        assert(type == inputs[1_gc_i].type());
        assert(type->aggregate_type() == gc::AggregateType::Scalar);
        gc::ScalarT(type).visit_numeric(
            [&](auto tag)
            {
                result.front() =
                    inputs[0_gc_i].as(tag) * inputs[1_gc_i].as(tag);
            });
        return true;
    }
};

auto make_multiply(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("Multiply", args);
    return std::make_shared<Multiply>();
}

} // namespace gc_app::num
