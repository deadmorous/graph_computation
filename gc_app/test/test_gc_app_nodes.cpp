/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/computation_node_registry.hpp"
#include "gc_app/nodes/cell_aut/cell2d.hpp"
#include "gc_app/nodes/cell_aut/gen_cmap_reader.hpp"
#include "gc_app/nodes/cell_aut/gen_rule_reader.hpp"
#include "gc_app/nodes/cell_aut/generate_cmap.hpp"
#include "gc_app/nodes/cell_aut/generate_rules.hpp"
#include "gc_app/nodes/cell_aut/life.hpp"
#include "gc_app/nodes/cell_aut/offset_image.hpp"
#include "gc_app/nodes/cell_aut/random_image.hpp"
#include "gc_app/nodes/cell_aut/rule_reader.hpp"
#include "gc_app/nodes/num/eratosthenes_sieve.hpp"
#include "gc_app/nodes/num/filter_seq.hpp"
#include "gc_app/nodes/num/multiply.hpp"
#include "gc_app/nodes/num/waring.hpp"
#include "gc_app/nodes/num/test_sequence.hpp"
#include "gc_app/nodes/util/project.hpp"
#include "gc_app/nodes/util/uint_size.hpp"
#include "gc_app/nodes/visual/image_colorizer.hpp"
#include "gc_app/nodes/visual/image_loader.hpp"
#include "gc_app/types/cell2d_gen_cmap.hpp"
#include "gc_app/types/cell2d_gen_rules.hpp"
#include "gc_app/types/cell2d_rules.hpp"
#include "gc_app/type_registry.hpp"

#include "gc_types/image.hpp"
#include "gc_types/palette.hpp"
#include "gc_types/uint_vec.hpp"

#include "gc/computation_context.hpp"
#include "gc/computation_node.hpp"
#include "gc/computation_node_registry.hpp"

#include "common/func_ref.hpp"

#include <gtest/gtest.h>


using namespace gc_app;
using namespace gc_types;
using namespace gc::literals;
using namespace std::literals;

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

auto make_computation_context() -> gc::ComputationContext
{
    auto context = gc::ComputationContext{
        .type_registry = gc::type_registry(),
        .node_registry = gc::computation_node_registry()
    };
    gc_app::populate_node_registry(context.node_registry);
    gc_app::populate_type_registry(context.type_registry);
    return context;
}

} // anonymous namespace


TEST(GcApp_Node, Cell2d)
{
    auto node = cell_aut::make_cell2d({}, {});

    ASSERT_EQ(node->input_count(), 2_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "rules");
    ASSERT_EQ(node->input_names()[1_gc_i], "input_state");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "output_state");

    gc::ValueVec inputs(2);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Cell2dRules>());
    ASSERT_EQ(inputs[1].type(), gc::type_of<I8Image>());

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<I8Image>());
}

TEST(GcApp_Node, GenCmapReader)
{
    auto node = cell_aut::make_gen_cmap_reader({}, {});

    ASSERT_EQ(node->input_count(), 1_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "file");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "gen_cmap");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<std::string>());

    inputs[0] = "data/program.cf"s;
    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<Cell2dGenCmap>());

    const auto& actual_gen_cmap = outputs[0].as<Cell2dGenCmap>();
    auto expected_gen_cmap = Cell2dGenCmap{
        .state_count = 256,
        .formula = {
            .r = "0",
            .g = "0",
            .b = "0"
        },
        .overlays = {
            {
                .formula = {
                    .r = "0",
                    .g = "0",
                    .b = "n*4",
                },
                .range = {
                    .min = 0,
                    .max = 63,
                    .step = 1
                }
            },
            {
                .formula = {
                    .r = "n*4",
                    .g = "n*4",
                    .b = "0",
                },
                .range = {
                    .min = 64,
                    .max = 127,
                    .step = 1
                }
            },
            {
                .formula = {
                    .r = "0",
                    .g = "n*4",
                    .b = "0",
                },
                .range = {
                    .min = 128,
                    .max = 191,
                    .step = 1
                }
            },
            {
                .formula = {
                    .r = "n*4",
                    .g = "0",
                    .b = "0",
                },
                .range = {
                    .min = 192,
                    .max = 255,
                    .step = 1
                }
            }
        }
    };

    EXPECT_EQ(actual_gen_cmap, expected_gen_cmap);
}

TEST(GcApp_Node, GenRuleReader)
{
    auto node = cell_aut::make_gen_rule_reader({}, {});

    ASSERT_EQ(node->input_count(), 1_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "file");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "gen_rules");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<std::string>());

    // Case 1 - read gen_rules
    {
        inputs[0] = "data/hunt.gen"s;
        node->compute_outputs(outputs, inputs, {}, {});
        ASSERT_EQ(outputs[0].type(), gc::type_of<Cell2dGenRules>());

        const auto& actual_gen_rules = outputs[0].as<Cell2dGenRules>();
        auto expected_gen_rules = Cell2dGenRules{
            .state_count = 128,
            .min_state = 0,
            .tor = true,
            .count_self = false,
            .map9{
                .formula = "4*n*(127-n/9)/1143"
            },
            .map6{},
            .map4{}
        };

        EXPECT_EQ(actual_gen_rules, expected_gen_rules);
    }

    // Case 2 - check if "count center cell" heuristics works
    {
        inputs[0] = "data/mid.gen"s;
        node->compute_outputs(outputs, inputs, {}, {});

        const auto& actual_gen_rules = outputs[0].as<Cell2dGenRules>();
        auto expected_gen_rules = Cell2dGenRules{
            .state_count = 128,
            .min_state = 0,
            .tor = false,
            .count_self = true,
            .map9{
                .formula = "n/9"
            },
            .map6{
                .formula = "n/6"
            },
            .map4{
                .formula = "n/4"
            }
        };

        EXPECT_EQ(actual_gen_rules, expected_gen_rules);
    }
}

TEST(GcApp_Node, GenerateRules)
{
    auto node = cell_aut::make_generate_rules({}, {});

    ASSERT_EQ(node->input_count(), 1_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "gen_rules");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "rules");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Cell2dGenRules>());

    inputs[0] = Cell2dGenRules{
        .state_count = 128,
        .min_state = 0,
        .tor = false,
        .count_self = true,
        .map9{
            .formula = "n/9"
        },
        .map6{
            .formula = "n/6"
        },
        .map4{
            .formula = "n/4"
        }
    };
    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<Cell2dRules>());
}

TEST(GcApp_Node, GenerateCmap)
{
    auto node = cell_aut::make_generate_cmap({}, {});

    ASSERT_EQ(node->input_count(), 1_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "gen_cmap");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "cmap");

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<Cell2dGenCmap>());

    inputs[0] = Cell2dGenCmap{
        .state_count = 256,
        .formula = {
            .r = "n",
            .g = "255-n",
            .b = "n/2"
        },
        .overlays = {
            {
                .formula = {
                    .r = "1",
                    .g = "2",
                    .b = "3"
                },
                .range = {
                    .min = 10,
                    .max = 20,
                    .step = 2
                }
            }
        }
    };
    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<IndexedColorMap>());

    const auto& cmap = outputs[0].as<IndexedColorMap>();
    ASSERT_EQ(cmap.size(), 256);
    using C = ColorComponent;
    for (size_t n : common::index_range<size_t>(256))
    {
        auto [r, g, b, a] = r_g_b_a(cmap[n]);
        EXPECT_EQ(a, C{0xff});
        if (n >= 10 && n <= 20 && n % 2 == 0)
        {
            EXPECT_EQ(r, C{1});
            EXPECT_EQ(g, C{2});
            EXPECT_EQ(b, C{3});
        }
        else
        {
            EXPECT_EQ(r, C(n));
            EXPECT_EQ(g, C(255 - n));
            EXPECT_EQ(b, C(n / 2));
        }
    }
}

TEST(GcApp_Node, Life)
{
    auto node = cell_aut::make_life({}, {});

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

TEST(GcApp_Node, OffsetImage)
{
    auto node = cell_aut::make_offset_image({}, {});

    ASSERT_EQ(node->input_count(), 2_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);

    ASSERT_EQ(node->input_names()[0_gc_i], "input_image"sv);
    ASSERT_EQ(node->input_names()[1_gc_i], "offset"sv);

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "output_image");

    gc::ValueVec inputs(2);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<I8Image>());
    ASSERT_EQ(inputs[1].type(), gc::type_of<int8_t>());

    auto& input_image = inputs[0].as<I8Image>();
    [[maybe_unused]]
    auto& offset = inputs[1].as<int8_t>();

    input_image = I8Image{
        .size{2, 2},
        .data{0, 1, -1, 2}
    };

    node->compute_outputs(outputs, inputs, {}, {});
    const auto& output_image = outputs[0].as<I8Image>();
    EXPECT_EQ(output_image.size, UintSize(2, 2));
    auto expected_output_pixels = std::vector<int8_t>{-1, 0, -2, 1};
    EXPECT_EQ(output_image.data, expected_output_pixels);
}

TEST(GcApp_Node, RandomImage)
{
    auto node = cell_aut::make_random_image({}, {});

    ASSERT_EQ(node->input_count(), 7_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 7_gc_ic);

    ASSERT_EQ(node->input_names()[0_gc_i], "size"sv);
    ASSERT_EQ(node->input_names()[1_gc_i], "lowest_state"sv);
    ASSERT_EQ(node->input_names()[2_gc_i], "range_size"sv);
    ASSERT_EQ(node->input_names()[3_gc_i], "map"sv);
    ASSERT_EQ(node->input_names()[4_gc_i], "radius"sv);
    ASSERT_EQ(node->input_names()[5_gc_i], "shape"sv);
    ASSERT_EQ(node->input_names()[6_gc_i], "outer_state"sv);

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "image");

    gc::ValueVec inputs(7);
    gc::ValueVec outputs(1);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0].type(), gc::type_of<UintSize>());
    ASSERT_EQ(inputs[1].type(), gc::type_of<int8_t>());
    ASSERT_EQ(inputs[2].type(), gc::type_of<int8_t>());
    ASSERT_EQ(inputs[3].type(), gc::type_of<std::vector<int8_t>>());
    ASSERT_EQ(inputs[4].type(), gc::type_of<int>());
    ASSERT_EQ(inputs[5].type(), gc::type_of<std::string>());
    ASSERT_EQ(inputs[6].type(), gc::type_of<int8_t>());

    ASSERT_EQ(inputs[0].as<UintSize>(), UintSize(100, 100));
    ASSERT_EQ(inputs[1].as<int8_t>(), int8_t{0});
    ASSERT_EQ(inputs[2].as<int8_t>(), int8_t{2});
    ASSERT_EQ(inputs[3].as<std::vector<int8_t>>(), std::vector<int8_t>{});
    ASSERT_EQ(inputs[4].as<int>(), -1);
    ASSERT_EQ(inputs[5].as<std::string>(), "circle");
    ASSERT_EQ(inputs[6].as<int8_t>(), int8_t{0});

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<I8Image>());

    const auto& img = outputs[0].as<I8Image>();
    EXPECT_EQ(img.size, UintSize(100, 100));
    size_t n0{};
    size_t n1{};
    for (auto pixel : img.data)
    {
        if (pixel == 0)
            ++n0;
        else if (pixel == 1)
            ++n1;
        else
            FAIL() << "Unexpected pixel value";
    }
    EXPECT_LT(n0, 2*n1);
    EXPECT_LT(n1, 2*n0);
}

TEST(GcApp_Node, RuleReader)
{
    auto node = cell_aut::make_rule_reader({}, {});

    ASSERT_EQ(node->input_count(), 1_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 1_gc_ic);

    ASSERT_EQ(node->input_names()[0_gc_i], "file"sv);

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "rules");
}

TEST(GcApp_Node, ImageColorizer)
{
    auto node = visual::make_image_colorizer({}, {});

    ASSERT_EQ(node->input_count(), 3_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 3_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "input_image");
    ASSERT_EQ(node->input_names()[1_gc_i], "palette");
    ASSERT_EQ(node->input_names()[2_gc_i], "min_state");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "output_image");

    gc::ValueVec inputs(3);
    gc::ValueVec outputs(1);

    using C = ColorComponent;
    constexpr auto black = rgba(C{0x00}, C{0x00}, C{0x00});
    constexpr auto white = rgba(C{0xff}, C{0xff}, C{0xff});
    constexpr auto green = rgba(C{0x00}, C{0xcc}, C{0x00});
    constexpr auto red   = rgba(C{0xcc}, C{0x00}, C{0x00});

    inputs[0] = Image<int8_t>{
        .size = {3, 2},
        .data = {0, 1, 2, -1, 3, 0}
    };
    inputs[1] = IndexedPalette{
        .color_map = { black, white, green },
        .overflow_color = red
    };
    inputs[2] = int8_t{0};

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<ColorImage>());

    const auto& image = outputs[0].as<ColorImage>();
    EXPECT_EQ(image.size, UintSize(3, 2));
    auto expected_pixels = std::vector<Color>{
        black, white, green, red, red, black
    };
    EXPECT_EQ(image.data, expected_pixels);
}

TEST(GcApp_Node, ImageLoader)
{
    auto node = visual::make_image_loader({}, {});

    ASSERT_EQ(node->input_count(), gc::InputPortCount{2});
    ASSERT_EQ(node->output_count(), gc::OutputPortCount{2});

    ASSERT_EQ(node->input_names().size(), 2_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "file");
    ASSERT_EQ(node->input_names()[1_gc_i], "min_state");

    ASSERT_EQ(node->output_names().size(), 2_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "image");
    ASSERT_EQ(node->output_names()[1_gc_o], "color_map");

    gc::ValueVec inputs(2);
    gc::ValueVec outputs(2);

    node->default_inputs(inputs);
    ASSERT_EQ(inputs[0], "image.png"s);
    ASSERT_EQ(inputs[1], int8_t{0});

    inputs[0] = "data/acorn.png"s;
    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].type(), gc::type_of<I8Image>());
    ASSERT_EQ(outputs[1].type(), gc::type_of<IndexedColorMap>());

    const auto& image = outputs[0].as<I8Image>();
    const auto& color_map = outputs[1].as<IndexedColorMap>();

    // Check image content
    constexpr size_t acorn_w = 7;
    constexpr size_t acorn_h = 3;
    int8_t acorn_data[acorn_h][acorn_w] = {
        {0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 1}
    };
    constexpr size_t acorn_x = 47;
    constexpr size_t acorn_y = 48;
    constexpr size_t w = 100;
    constexpr size_t h = 100;
    ASSERT_EQ(image.size.width, w);
    ASSERT_EQ(image.size.height, h);
    ASSERT_EQ(image.data.size(), w*h);
    const auto* pixel_ptr = image.data.data();
    for (size_t y=0; y<h; ++y)
    {
        for (size_t x=0; x<w; ++x, ++pixel_ptr)
        {
            auto actual = *pixel_ptr;
            auto expected = [&]() -> int8_t {
                if (y < acorn_y || y >= acorn_y + acorn_h ||
                    x < acorn_x || x >= acorn_x + acorn_w)
                    return 0;
                return acorn_data[y-acorn_y][x-acorn_x];
            }();
            EXPECT_EQ(actual, expected);
        }
    }

    // Check color map content
    using C = ColorComponent;
    ASSERT_EQ(color_map.size(), 2);
    EXPECT_EQ(color_map[0], rgba(C{0}, C{0}, C{0}));
    EXPECT_EQ(color_map[1], rgba(C{0xff}, C{0xff}, C{0xff}));
}

TEST(GcApp_Node, EratosthenesSieve)
{
    auto node = num::make_eratosthenes_sieve({}, {});

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
    auto node = num::make_filter_seq({}, {});

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
    auto node = gc_app::num::make_test_sequence({}, {});

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
    auto node = gc_app::num::make_multiply({}, {});

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

TEST(GcApp_Node, Merge)
{
    auto context = make_computation_context();
    context.type_registry.register_value(
        "Vector[I32]", gc::type_of<std::vector<int32_t>>());

    auto input_count = gc::Value{common::Type<size_t>, 2u};
    auto node = context.node_registry.at("merge")({&input_count, 1}, context);

    ASSERT_EQ(node->input_count(), 5_gc_ic);
    ASSERT_EQ(node->output_count(), 1_gc_oc);

    ASSERT_EQ(node->input_names().size(), 5_gc_ic);
    ASSERT_EQ(node->input_names()[0_gc_i], "output_type");
    ASSERT_EQ(node->input_names()[1_gc_i], "path_0");
    ASSERT_EQ(node->input_names()[2_gc_i], "value_0");
    ASSERT_EQ(node->input_names()[3_gc_i], "path_1");
    ASSERT_EQ(node->input_names()[4_gc_i], "value_1");

    ASSERT_EQ(node->output_names().size(), 1_gc_oc);
    ASSERT_EQ(node->output_names()[0_gc_o], "output");

    auto output_type = "IndexedPalette"s;
    auto path_0 = gc::ValuePath{} / "color_map"sv;
    using C = ColorComponent;
    auto value_0 = IndexedColorMap{
        rgba(C{0x00}, C{0x00}, C{0x00}),
        rgba(C{0xff}, C{0xff}, C{0xff}),
    };
    auto path_1 = gc::ValuePath{} / "overflow_color"sv;
    auto value_1 = rgba(C{0xcc}, C{0x00}, C{0x00});
    auto inputs = gc::ValueVec{ output_type, path_0, value_0, path_1, value_1 };
    gc::ValueVec outputs(1);

    node->compute_outputs(outputs, inputs, {}, {});

    auto const& palette = outputs[0].as<IndexedPalette>();
    ASSERT_EQ(palette.color_map, value_0);
    ASSERT_EQ(palette.overflow_color, value_1);
}

TEST(GcApp_Node, Project)
{
    auto node = gc_app::util::make_project({}, {});

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
    auto node = util::make_uint_size({}, {});

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

    auto expected_size = gc_types::UintSize{ 800, 600 };
    inputs[0] = expected_size.width;
    inputs[1] = expected_size.height;

    node->compute_outputs(outputs, inputs, {}, {});
    ASSERT_EQ(outputs[0].as<UintSize>(), expected_size);
}

TEST(GcApp_Node, Waring)
{
    auto node = gc_app::num::make_waring({}, {});

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
    auto node = num::make_eratosthenes_sieve({}, {});

    gc::ValueVec inputs(1);
    gc::ValueVec outputs(1);

    auto progress_checker = ProgressChecker{};

    inputs[0] = uint_val(10'000'000);
    node->compute_outputs(outputs, inputs, {}, &progress_checker);

    progress_checker.check();
}

TEST(GcApp_Progress, Waring)
{
    auto node = num::make_waring({}, {});

    gc::ValueVec inputs(3);
    gc::ValueVec outputs(1);

    auto progress_checker = ProgressChecker{};

    inputs[0] = uint_val(10'000);
    inputs[1] = uint_val(3);
    inputs[2] = uint_val(2);
    node->compute_outputs(outputs, inputs, {}, &progress_checker);

    progress_checker.check();
}
