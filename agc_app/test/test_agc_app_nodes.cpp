#include "agc_app/nodes/canvas.hpp"
#include "agc_app/nodes/counter.hpp"
#include "agc_app/nodes/func_iterator.hpp"
#include "agc_app/nodes/grid_2d.hpp"
#include "agc_app/nodes/linspace.hpp"
#include "agc_app/nodes/mag2.hpp"
#include "agc_app/nodes/mandelbrot_func.hpp"
#include "agc_app/nodes/printer.hpp"
#include "agc_app/nodes/replicate.hpp"
#include "agc_app/nodes/scale.hpp"
#include "agc_app/nodes/split.hpp"
#include "agc_app/nodes/threshold.hpp"

#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/value.hpp"

#include <gtest/gtest.h>


using namespace agc_app;
using namespace gc::literals;
using namespace std::string_view_literals;

namespace {

auto print_node_algos(const gc::ActivationNode* node)
    -> void
{
    auto alg_storage = gc::alg::AlgorithmStorage{};
    auto algos = node->activation_algorithms(alg_storage);
    std::cout
        << gc::PrintableNodeActivationAlgorithms{algos, alg_storage}
        << "\n=====\n";
}

template <std::same_as<std::string_view>... Ts>
auto make_source_types(gc::alg::AlgorithmStorage& alg_storage, Ts...types)
    -> gc::ActivationGraphSourceTypes
{
    auto result = gc::ActivationGraphSourceTypes{};

    auto port = gc::InputPort{0};
    ([&](std::string_view type)
    {
        result.types.push_back(
            alg_storage(gc::alg::Type{ .name = std::string(type) }));
        add_to_last_group(result.destinations,
                          gc::EdgeInputEnd{ 0_gc_n, port });
        next_group(result.destinations);
        ++port;
    }(types), ...);

    return result;
}

template <std::same_as<std::string_view>... Ts>
auto print_node_source_code(gc::ActivationNodePtr node, Ts...types)
    -> void
{
    auto alg_storage = gc::alg::AlgorithmStorage{};
    auto source_types = make_source_types(alg_storage, types...);

    gc::generate_source(
        std::cout,
        gc::ActivationGraph{ .nodes = { node } },
        alg_storage,
        source_types);
}

} // anonymous namespace

TEST(AgcApp_Node, Canvas)
{
    auto node = make_canvas({});
    EXPECT_EQ(node->input_count(), 5_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "size"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "set"sv);
    EXPECT_EQ(node->input_names()[2_gc_i], "set_next"sv);
    EXPECT_EQ(node->input_names()[3_gc_i], "flush"sv);
    EXPECT_EQ(node->input_names()[4_gc_i], "clear"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "canvas"sv);

    print_node_algos(node.get());
    print_node_source_code(node);
}

TEST(AgcApp_Node, Counter)
{
    auto node = make_counter({});
    EXPECT_EQ(node->input_count(), 2_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "next"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "reset"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "count"sv);

    print_node_algos(node.get());
    print_node_source_code(node);
}

TEST(AgcApp_Node, FuncIterator)
{
    auto node = make_func_iterator({});
    EXPECT_EQ(node->input_count(), 3_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "init"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "value"sv);
    EXPECT_EQ(node->input_names()[2_gc_i], "next"sv);
    EXPECT_EQ(node->output_count(), 2_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "arg"sv);
    EXPECT_EQ(node->output_names()[1_gc_o], "value"sv);

    print_node_algos(node.get());
    print_node_source_code(node, "double"sv, "double"sv);
}

TEST(AgcApp_Node, Grid2d)
{
    auto node = make_grid_2d({});
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "spec"sv);
    EXPECT_EQ(node->output_count(), 2_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "grid_size"sv);
    EXPECT_EQ(node->output_names()[1_gc_o], "point"sv);

    print_node_algos(node.get());
    print_node_source_code(node);
}

TEST(AgcApp_Node, LinSpace)
{
    auto node = make_linspace({});
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "spec"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "sequence"sv);

    print_node_algos(node.get());
    print_node_source_code(node);
}

TEST(AgcApp_Node, Mag2)
{
    auto node = make_mag2({});
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "value"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "mag2"sv);

    print_node_algos(node.get());
    print_node_source_code(node);
}

TEST(AgcApp_Node, MandelbrotFunc)
{
    auto node = make_mandelbrot_func({});
    EXPECT_EQ(node->input_count(), 2_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "c"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "z"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "value"sv);

    print_node_algos(node.get());
    print_node_source_code(node);
}

TEST(AgcApp_Node, Printer)
{
    auto node = make_printer({});
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "value"sv);
    EXPECT_EQ(node->output_count(), common::Zero);

    print_node_algos(node.get());
    print_node_source_code(node, "int"sv);
}

TEST(AgcApp_Node, Replicate)
{
    auto node = make_replicate({});
    EXPECT_EQ(node->input_count(), 2_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "value"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "trigger"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "value"sv);

    print_node_algos(node.get());
    print_node_source_code(node, "float"sv);
}

TEST(AgcApp_Node, Scale)
{
    auto node = make_scale({});
    EXPECT_EQ(node->input_count(), 2_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "factor"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "value"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "scaled"sv);

    print_node_algos(node.get());
    print_node_source_code(node, "double"sv, "int"sv);
}

TEST(AgcApp_Node, Split)
{
    auto split_args = std::vector<gc::Value>{3};
    auto node = make_split(split_args);
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "in"sv);
    EXPECT_EQ(node->output_count(), 3_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "out_0"sv);
    EXPECT_EQ(node->output_names()[1_gc_o], "out_1"sv);
    EXPECT_EQ(node->output_names()[2_gc_o], "out_2"sv);

    print_node_algos(node.get());
    print_node_source_code(node, "int"sv);
}

TEST(AgcApp_Node, Threshold)
{
    auto node = make_threshold({});
    EXPECT_EQ(node->input_count(), 2_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "threshold"sv);
    EXPECT_EQ(node->input_names()[1_gc_i], "value"sv);
    EXPECT_EQ(node->output_count(), 2_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "pass"sv);
    EXPECT_EQ(node->output_names()[1_gc_o], "fail"sv);

    print_node_algos(node.get());
    print_node_source_code(node, "double"sv, "double"sv);
}
