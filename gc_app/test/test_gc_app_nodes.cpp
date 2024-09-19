#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/source_param.hpp"
#include "gc_app/test_sequence.hpp"
#include "gc_app/types.hpp"

#include <gtest/gtest.h>


using namespace gc_app;

TEST(GcApp, EratosthenesSieve)
{
    EratosthenesSieve node;

    ASSERT_EQ(node.input_count(), 1);
    ASSERT_EQ(node.output_count(), 1);

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node.default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Uint>());

    auto count = uint_val(inputs[0]);
    ASSERT_GT(count, 1);
    ASSERT_LE(count, 10000);

    inputs[0] = uint_val(10);
    node.compute_outputs(outputs, inputs);
    ASSERT_EQ(outputs[0].type(), gc::type_of<UintVec>());

    const auto& actual_output = uint_vec_val(outputs[0]);
    const auto expected_output =
        //       0  1  2  3  4  5  6  7  8  9
        UintVec{ 0, 0, 0, 0, 1, 0, 2, 0, 1, 1 };
    ASSERT_EQ(actual_output, expected_output);
}

TEST(GcApp, TestSequence)
{
    TestSequence node;

    ASSERT_EQ(node.input_count(), 1);
    ASSERT_EQ(node.output_count(), 1);

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node.default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Uint>());

    auto count = uint_val(inputs[0]);
    ASSERT_GT(count, 1);
    ASSERT_LE(count, 10000);

    inputs[0] = uint_val(10);
    node.compute_outputs(outputs, inputs);
    ASSERT_EQ(outputs[0].type(), gc::type_of<UintVec>());

    const auto& actual_output = uint_vec_val(outputs[0]);
    const auto expected_output =
        //       0  1  2  3  4  5  6  7  8  9
        UintVec{ 0, 0, 1, 0, 1, 1, 0, 1, 1, 1 };
    ASSERT_EQ(actual_output, expected_output);
}

TEST(GcApp, SourceParam)
{
    SourceParam node;

    ASSERT_EQ(node.input_count(), 0);
    ASSERT_EQ(node.output_count(), 0);

    gc::ValueVec inputs = { 12, 3.4 };

    auto check = [&](gc::ConstValueSpan s)
    {
        ASSERT_EQ(s.size(), inputs.size());
        ASSERT_EQ(s[0].as<int>(), 12);
        ASSERT_EQ(s[1].as<double>(), 3.4);
    };

    node.set_inputs(inputs);
    ASSERT_EQ(node.output_count(), 2);

    gc::ValueVec outputs(2);
    node.compute_outputs(outputs, {});
    check(outputs);

    gc::ValueVec inputs_copy(2);
    node.get_inputs(inputs_copy);
    check(inputs_copy);
}

TEST(GcApp, Multiply)
{
    Multiply node;

    ASSERT_EQ(node.input_count(), 2);
    ASSERT_EQ(node.output_count(), 1);

    auto check =
        [&]<typename T>(T a, T b)
    {
        gc::ValueVec inputs{ a, b };
        gc::ValueVec outputs(1);

        node.compute_outputs(outputs, inputs);
        ASSERT_EQ(outputs[0].as<T>(), a*b);
    };

    check(2, 3);
    check(1.2, 3.4);
}
