#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/filter_seq.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/test_sequence.hpp"
#include "gc_app/waring.hpp"
#include "gc_app/types.hpp"

#include "gc/node.hpp"

#include "common/func_ref.hpp"

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
    node->compute_outputs(outputs, inputs, {}, {});
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
            node->compute_outputs(outputs, inputs, {}, {});
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
    node->compute_outputs(outputs, inputs, {}, {});
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

        node->compute_outputs(outputs, inputs, {}, {});
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

        node->compute_outputs(outputs, inputs, {}, {});
        ASSERT_EQ(outputs[0].as<P>(), projection);
    };

    check(std::vector<int>{123, 45}, gc::ValuePath{}/0u, 123);
    check(std::vector<int>{123, 45}, gc::ValuePath{}/1u, 45);
}

TEST(GcApp, Waring)
{
    auto node = gc_app::make_waring({});

    ASSERT_EQ(node->input_count(), 3);
    ASSERT_EQ(node->output_count(), 1);

    ASSERT_EQ(node->input_names().size(), 3);
    ASSERT_EQ(node->input_names()[0], "count");
    ASSERT_EQ(node->input_names()[1], "s");
    ASSERT_EQ(node->input_names()[2], "k");

    ASSERT_EQ(node->output_names().size(), 1);
    ASSERT_EQ(node->output_names()[0], "sequence");

    auto check = [&](Uint count,
                    Uint s, Uint k,
                    std::vector<std::pair<Uint, UintVec>> expected)
    {
        gc::ValueVec inputs{ count, s, k };
        gc::ValueVec outputs(1);

        node->compute_outputs(outputs, inputs, {}, {});
        const auto& seq = outputs[0].as<UintVec>();
        // for (size_t i=0, n=seq.size(); i<n; ++i)
        //     std::cout << i << '\t' << seq[i] << std::endl;

        EXPECT_EQ(seq.size(), count);
        auto actual = std::vector<std::pair<Uint, UintVec>>{};
        Uint v = 1;
        while(true)
        {
            auto vi = UintVec{};
            for (Uint i=0, n=seq.size(); i<n; ++i)
                if (seq[i] == v)
                    vi.push_back(i);
            if (vi.empty())
                break;
            actual.push_back({v, std::move(vi)});
            ++v;
        }
        EXPECT_EQ(actual, expected);
    };

    check(100, 1, 2, {{1, {0, 1, 4, 9, 16, 25, 36, 49, 64, 81}}});

    check(35, 2, 2,
          {{1, {0, 1, 2, 4, 5, 8, 9, 10, 13, 16, 17, 18, 20, 26, 29, 32, 34}},
           {2, {25}}});

    check(22, 3, 2,
          {{1, {0, 1, 2, 3, 4, 5, 6, 8, 10, 11, 12, 13, 14, 16, 19, 20, 21}},
           {2, {9, 17, 18}}});

    check(100, 1, 3,
          {{1, {0, 1, 8, 27, 64}}});

    check(100, 2, 3,
          {{1, {0, 1, 2, 8, 9, 16, 27, 28, 35, 54, 64, 65, 72, 91}}});

    check(36, 3, 3,
          {{1, {0, 1, 2, 3, 8, 9, 10, 16, 17, 24, 27, 28, 29, 35}}});
}
