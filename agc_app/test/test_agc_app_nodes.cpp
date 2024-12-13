#include "agc_app/nodes/linspace.hpp"
#include "agc_app/nodes/printer.hpp"

#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"

#include <gtest/gtest.h>


using namespace agc_app;
using namespace gc::literals;
using namespace std::string_view_literals;


TEST(GcApp_Node, LinSpace)
{
    auto node = make_linspace({});
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "spec"sv);
    EXPECT_EQ(node->output_count(), 1_gc_oc);
    EXPECT_EQ(node->output_names()[0_gc_o], "sequence"sv);

    auto alg_storage = gc::alg::AlgorithmStorage{};
    auto algs = node->activation_algorithms(alg_storage);
    std::cout
        << gc::PrintableNodeActivationAlgorithms{algs, alg_storage}
        << std::endl;
}

TEST(GcApp_Node, Printer)
{
    auto node = make_printer({});
    EXPECT_EQ(node->input_count(), 1_gc_ic);
    EXPECT_EQ(node->input_names()[0_gc_i], "value"sv);
    EXPECT_EQ(node->output_count(), common::Zero);

    auto alg_storage = gc::alg::AlgorithmStorage{};
    auto algs = node->activation_algorithms(alg_storage);
    std::cout
        << gc::PrintableNodeActivationAlgorithms{algs, alg_storage}
        << std::endl;
}
