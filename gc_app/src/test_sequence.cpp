/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/test_sequence.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app {
namespace {

auto test_seq(Uint limit)
    -> UintVec
{
    assert(limit > 0);
    auto result = UintVec(limit, 1);
    result[0] = 0;
    for (size_t index=0, n=1; index<limit; index+=n, n+=1)
        result[index] = 0;
    return result;
}
} // anonymous namespace


class TestSequence final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<TestSequence>( "count"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<TestSequence>( "sequence"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = uint_val(1000);
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 1_gc_ic);
        assert(result.size() == 1_gc_oc);
        auto count = uint_val(inputs[0_gc_i]);
        result[0_gc_o] = uint_vec_val(test_seq(count));
        return true;
    }
};

auto make_test_sequence(gc::ConstValueSpan arg)
    -> std::shared_ptr<gc::ComputationNode>
{
    assert(arg.empty());
    return std::make_shared<TestSequence>();
}

} // namespace gc_app
