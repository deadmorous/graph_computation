/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/cell_aut/life.hpp"
#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/filter_seq.hpp"
#include "gc_app/image.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/test_sequence.hpp"
#include "gc_app/uint_size.hpp"
#include "gc_app/waring.hpp"
#include "gc_app/types.hpp"

#include "gc/computation_node.hpp"

#include "common/func_ref.hpp"

#include <gtest/gtest.h>


using namespace gc_app;
using namespace gc::literals;

namespace {

class ProgressChecker final
{
public:
    ProgressChecker() :
        t_{ std::chrono::steady_clock::now() }
    {}

    auto operator()(double progress_value)
        -> void
    {
        auto t = std::chrono::steady_clock::now();
        auto dt = t - t_;
        auto dp = progress_value - last_progress_value_;
        t_ = t;
        last_progress_value_ = progress_value;
        ++count_;

        EXPECT_GE(progress_value, 0.);
        EXPECT_LE(progress_value, 1.);

        if (count_ == 1)
            // Skip fisrt interval
            return;

        if (count_ == 2)
        {
            shortest_interval_ = longest_interval_ = dt;
            shortest_progress_delta_ = longest_progress_delta_ = dp;
            return;
        }

        shortest_interval_ = std::min(shortest_interval_, dt);
        longest_interval_ = std::max(longest_interval_, dt);
        shortest_progress_delta_ = std::min(shortest_progress_delta_, dp);
        longest_progress_delta_ = std::min(longest_progress_delta_, dp);
    }

    auto check() const
    {
        EXPECT_GE(count_, 1);

        if (count_ < 2)
            return;

        EXPECT_GT(shortest_progress_delta_, 0);
        EXPECT_GT(shortest_progress_delta_/longest_progress_delta_, 0.1);
        EXPECT_GT(static_cast<double>(shortest_interval_.count())/
                      longest_interval_.count(), 0.1);
    }

private:
    std::chrono::steady_clock::time_point t_;
    size_t count_{};
    double last_progress_value_{};
    std::chrono::nanoseconds shortest_interval_;
    std::chrono::nanoseconds longest_interval_;
    double shortest_progress_delta_;
    double longest_progress_delta_;
};

} // anonymous namespace


TEST(GcApp_Node, Life)
{
    auto node = cell_aut::make_life({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{1});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "input");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "output");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<I8Image>());

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<I8Image>());
}

TEST(GcApp_Node, EratosthenesSieve)
{
    auto node = make_eratosthenes_sieve({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{1});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "count");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "sequence");

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

TEST(GcApp_Node, FilterSeq)
{
    auto node = make_filter_seq({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{2});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "sequence");
    ASSERT_EQ(node->input_names()[1_gc_i], "value");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "indices");

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

TEST(GcApp_Node, TestSequence)
{
    auto node = gc_app::make_test_sequence({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{1});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "count");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "sequence");

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

TEST(GcApp_Node, Multiply)
{
    auto node = gc_app::make_multiply({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{2});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "lhs");
    ASSERT_EQ(node->input_names()[1_gc_i], "rhs");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "product");

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

TEST(GcApp_Node, Project)
{
    auto node = gc_app::make_project({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{2});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "value");
    ASSERT_EQ(node->input_names()[1_gc_i], "path");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "projection");

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

TEST(GcApp_Node, UintSizeNode)
{
    auto node = make_uint_size({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{2});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "width");
    ASSERT_EQ(node->input_names()[1_gc_i], "height");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "size");

    gc::ValueVec inputs(2);
    gc::ValueVec outputs(1);
    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Uint>());
    ASSERT_EQ(inputs[1].type(), gc::type_of<Uint>());

    auto expected_size = gc_app::UintSize{ 800, 600 };
    inputs[0] = expected_size.width;
    inputs[1] = expected_size.height;

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].as<UintSize>(), expected_size);
}

TEST(GcApp_Node, Waring)
{
    auto node = gc_app::make_waring({});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{3});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{1});

    ASSERT_EQ(node->input_names().size(), 3_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "count");
    ASSERT_EQ(node->input_names()[1_gc_i], "s");
    ASSERT_EQ(node->input_names()[2_gc_i], "k");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "sequence");

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

// ---

TEST(GcApp_Progress, EratosthenesSieve)
{
    auto node = make_eratosthenes_sieve({});

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    auto progress_checker = ProgressChecker{};

    inputs[0] = uint_val(10'000'000);
    node->compute_outputs(outputs, inputs, {}, &progress_checker);

    progress_checker.check();
}

TEST(GcApp_Progress, Waring)
{
    auto node = make_waring({});

    gc::ValueVec inputs(3);
    gc::ValueVec outputs(1);

    auto progress_checker = ProgressChecker{};

    inputs[0] = uint_val(10'000);
    inputs[1] = uint_val(3);
    inputs[2] = uint_val(2);
    node->compute_outputs(outputs, inputs, {}, &progress_checker);

    progress_checker.check();
}
