/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "sieve/nodes/i8_image_metrics.hpp"
#include "sieve/types/image_metrics.hpp"

#include "gc_app/types/image.hpp"

#include "gc/computation_context.hpp"
#include "gc/computation_node.hpp"

#include "common/func_ref.hpp"

#include <gtest/gtest.h>


using namespace sieve;
using namespace gc::literals;
using namespace std::literals;

TEST(Sieve_Node, ImageMetrics)
{
    auto node = make_i8_image_metrics({}, {});

    ASSERT_EQ(node->input_count(), 3_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 3_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "image");
    ASSERT_EQ(node->input_names()[1_gc_i], "min_state");
    ASSERT_EQ(node->input_names()[2_gc_i], "state_count");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "image_metrics");

    gc::ValueVec inputs(3);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<gc_app::I8Image>());
    ASSERT_EQ(inputs[1].type(), gc::type_of<int>());
    ASSERT_EQ(inputs[2].type(), gc::type_of<int>());

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<ImageMetrics>());
}
