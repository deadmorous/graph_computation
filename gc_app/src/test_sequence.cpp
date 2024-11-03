#include "gc_app/test_sequence.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;

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
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<TestSequence>( "count"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<TestSequence>( "sequence"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    {
        assert(result.size() == 1);
        result[0] = uint_val(1000);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 1);
        assert(result.size() == 1);
        auto count = uint_val(inputs[0]);
        result[0] = uint_vec_val(test_seq(count));
        return true;
    }
};

auto make_test_sequence(gc::ConstValueSpan arg)
    -> std::shared_ptr<gc::Node>
{
    assert(arg.empty());
    return std::make_shared<TestSequence>();
}

} // namespace gc_app
