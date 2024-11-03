#include "gc/source_param.hpp"

#include "gc/value.hpp"

#include "common/func_ref.hpp"

#include <gtest/gtest.h>


TEST(Gc, SourceParam)
{
    auto node = gc::make_source_param(gc::ValueVec{ 11, 2.2 });

    ASSERT_EQ(node->input_count(), 0);
    ASSERT_EQ(node->output_count(), 2);

    ASSERT_EQ(node->input_names().size(), 0);

    ASSERT_EQ(node->output_names().size(), 2);
    ASSERT_EQ(node->output_names()[0], "out_0");
    ASSERT_EQ(node->output_names()[1], "out_1");

    // ---

    gc::ValueVec inputs = { 12, 3.4 };

    auto check = [&](gc::ConstValueSpan s)
    {
        ASSERT_EQ(s.size(), inputs.size());
        ASSERT_EQ(s[0].as<int>(), 12);
        ASSERT_EQ(s[1].as<double>(), 3.4);
    };

    auto* param = gc::InputParameters::get(node.get());
    param->set_inputs(inputs);

    // ---

    gc::ValueVec outputs(2);
    node->compute_outputs(outputs, {}, {}, {});
    check(outputs);

    gc::ValueVec inputs_copy(2);
    param->get_inputs(inputs_copy);
    check(inputs_copy);

    // Number of parameters in specified at construction time.
    EXPECT_THROW(
        param->set_inputs(gc::ValueVec{1, 2, 3}),
        std::invalid_argument);
}
