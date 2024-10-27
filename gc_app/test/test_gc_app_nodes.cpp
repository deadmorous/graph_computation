#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/filter_seq.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/test_sequence.hpp"
#include "gc_app/types.hpp"

#include "gc/node.hpp"

#include <gtest/gtest.h>


using namespace gc_app;

TEST(GcApp, EratosthenesSieve)
{
    auto node = make_eratosthenes_sieve({});

    ASSERT_EQ(node->input_count(), 1);
    ASSERT_EQ(node->output_count(), 1);

    ASSERT_EQ(node->input_names().size(), 1);
    ASSERT_EQ(node->input_names()[0], "count");

    ASSERT_EQ(node->output_names().size(), 1);
    ASSERT_EQ(node->output_names()[0], "sequence");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Uint>());

    auto count = uint_val(inputs[0]);
    ASSERT_GT(count, 1);
    ASSERT_LE(count, 10000);

    inputs[0] = uint_val(10);
    node->compute_outputs(outputs, inputs);
    ASSERT_EQ(outputs[0].type(), gc::type_of<UintVec>());

    const auto& actual_output = uint_vec_val(outputs[0]);
    const auto expected_output =
        //       0  1  2  3  4  5  6  7  8  9
        UintVec{ 0, 0, 0, 0, 1, 0, 2, 0, 1, 1 };
    ASSERT_EQ(actual_output, expected_output);
}

TEST(GcApp, FilterSeq)
{
    auto node = make_filter_seq({});

    ASSERT_EQ(node->input_count(), 2);
    ASSERT_EQ(node->output_count(), 1);

    ASSERT_EQ(node->input_names().size(), 2);
    ASSERT_EQ(node->input_names()[0], "sequence");
    ASSERT_EQ(node->input_names()[1], "value");

    ASSERT_EQ(node->output_names().size(), 1);
    ASSERT_EQ(node->output_names()[0], "indices");

    gc::ValueVec inputs(2);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<UintVec>());
    ASSERT_EQ(inputs[1].type(), gc::type_of<Uint>());

    auto input_seq = uint_vec_val(inputs[0]);
    ASSERT_GT(input_seq.size(), 1);
    ASSERT_LE(input_seq.size(), 100);

    //                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12
    inputs[0] = uint_vec_val({0, 1, 2, 0, 2, 1, 5, 2, 1, 6, 1, 0, 2});

    auto test_filter_value =
        [&](Uint value, UintVec expected_output)
    {
        inputs[1] = uint_val(value);
        node->compute_outputs(outputs, inputs);
        ASSERT_EQ(outputs[0].type(), gc::type_of<UintVec>());

        const auto& actual_output = uint_vec_val(outputs[0]);
        ASSERT_EQ(actual_output, expected_output);
    };

    test_filter_value(0, { 0, 3, 11 });
    test_filter_value(1, { 1, 5, 8, 10 });
    test_filter_value(2, { 2, 4, 7, 12 });
    test_filter_value(3, {});
    test_filter_value(4, {});
    test_filter_value(5, { 6 });
    test_filter_value(6, { 9 });
    test_filter_value(7, {});
}

TEST(GcApp, TestSequence)
{
    auto node = gc_app::make_test_sequence({});

    ASSERT_EQ(node->input_count(), 1);
    ASSERT_EQ(node->output_count(), 1);

    ASSERT_EQ(node->input_names().size(), 1);
    ASSERT_EQ(node->input_names()[0], "count");

    ASSERT_EQ(node->output_names().size(), 1);
    ASSERT_EQ(node->output_names()[0], "sequence");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Uint>());

    auto count = uint_val(inputs[0]);
    ASSERT_GT(count, 1);
    ASSERT_LE(count, 10000);

    inputs[0] = uint_val(10);
    node->compute_outputs(outputs, inputs);
    ASSERT_EQ(outputs[0].type(), gc::type_of<UintVec>());

    const auto& actual_output = uint_vec_val(outputs[0]);
    const auto expected_output =
        //       0  1  2  3  4  5  6  7  8  9
        UintVec{ 0, 0, 1, 0, 1, 1, 0, 1, 1, 1 };
    ASSERT_EQ(actual_output, expected_output);
}

TEST(GcApp, Multiply)
{
    auto node = gc_app::make_multiply({});

    ASSERT_EQ(node->input_count(), 2);
    ASSERT_EQ(node->output_count(), 1);

    ASSERT_EQ(node->input_names().size(), 2);
    ASSERT_EQ(node->input_names()[0], "lhs");
    ASSERT_EQ(node->input_names()[1], "rhs");

    ASSERT_EQ(node->output_names().size(), 1);
    ASSERT_EQ(node->output_names()[0], "product");

    auto check =
        [&]<typename T>(T a, T b)
    {
        gc::ValueVec inputs{ a, b };
        gc::ValueVec outputs(1);

        node->compute_outputs(outputs, inputs);
        ASSERT_EQ(outputs[0].as<T>(), a*b);
    };

    check(2, 3);
    check(1.2, 3.4);
}

TEST(GcApp, Project)
{
    auto node = gc_app::make_project({});

    ASSERT_EQ(node->input_count(), 2);
    ASSERT_EQ(node->output_count(), 1);

    ASSERT_EQ(node->input_names().size(), 2);
    ASSERT_EQ(node->input_names()[0], "value");
    ASSERT_EQ(node->input_names()[1], "path");

    ASSERT_EQ(node->output_names().size(), 1);
    ASSERT_EQ(node->output_names()[0], "projection");

    auto check =
        [&]<typename T, typename P>(T value, gc::ValuePath path, P projection)
    {
        gc::ValueVec inputs{ value, path };
        gc::ValueVec outputs(1);

        node->compute_outputs(outputs, inputs);
        ASSERT_EQ(outputs[0].as<P>(), projection);
    };

    check(std::vector<int>{123, 45}, gc::ValuePath{}/0u, 123);
    check(std::vector<int>{123, 45}, gc::ValuePath{}/1u, 45);
}
