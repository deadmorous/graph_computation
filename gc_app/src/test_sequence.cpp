#include "gc_app/test_sequence.hpp"

#include "gc_app/types.hpp"


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
    auto input_count() const
        -> uint32_t
    { return 1; }

    auto output_count() const
        -> uint32_t
    { return 1; }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 1);
        result[0] = uint_val(1000);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 1);
        assert(result.size() == 1);
        auto count = uint_val(inputs[0]);
        result[0] = uint_vec_val(test_seq(count));
    }
};

auto make_test_sequence()
    -> std::shared_ptr<gc::Node>
{ return std::make_shared<TestSequence>(); }

} // namespace gc_app
