/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/node_registry.hpp"
#include "gc_app/type_registry.hpp"
#include "gc_app/types/image.hpp"

#include "gc/computation_node_registry.hpp"
#include "gc/graph_computation.hpp"
#include "gc/yaml/parse_graph.hpp"

#include <yaml-cpp/yaml.h>

#include <gtest/gtest.h>


using namespace gc::literals;

TEST(GcApp, ParseGraph)
{
    // Graph definition in the YAML format
    constexpr auto* config_text = R"(
nodes:
  - name: img_size
    type: uint_size

  - name: seq_size
    type: multiply

  - name: sieve
    type: eratosthenes_sieve

  - name: view
    type: rect_view

edges:
  - [seq_size.product,    sieve.count]
  - [img_size,            view.size]
  - [sieve.sequence,      view.sequence]

inputs:
  - name: img_width
    type: U32
    value: 600
    destinations: [img_size.width, seq_size.lhs]
  - name: img_height
    type: U32
    value: 500
    destinations: [img_size.height, seq_size.rhs]
  - name: palette
    type: IndexedPalette
    value:
      color_map: [0xffffffff]
      overflow_color: 0xff000000
    destinations: [view.palette]
)";

    // Initialize node registry and type registry
    auto node_registry = gc::computation_node_registry();
    gc_app::populate_node_registry(node_registry);

    auto type_registry = gc::type_registry();
    gc_app::populate_type_registry(type_registry);

    // Parse YAML into a node object; parse graph from that node
    auto config = YAML::Load(config_text);
    auto [g, provided_inputs, node_map, input_names] =
        gc::yaml::parse_graph(config, node_registry, type_registry);

    // Check number of nodes and edges
    EXPECT_EQ(g.nodes.size(), 4_gc_nc);
    EXPECT_EQ(g.edges.size(), 3);

    // Check that nodes in the graph are in the same order as in
    // the YAML file; check `node_map`.
    EXPECT_EQ(g.nodes.at(0_gc_n).get(), node_map.at("img_size"));
    EXPECT_EQ(g.nodes.at(1_gc_n).get(), node_map.at("seq_size"));
    EXPECT_EQ(g.nodes.at(2_gc_n).get(), node_map.at("sieve"));
    EXPECT_EQ(g.nodes.at(3_gc_n).get(), node_map.at("view"));

    // Check source inputs
    EXPECT_EQ(provided_inputs.values.size(), 3);
    EXPECT_EQ(provided_inputs.values[0], uint32_t{600});
    auto in0_dst = group(provided_inputs.destinations, 0);
    EXPECT_EQ(in0_dst.size(), 2);
    EXPECT_EQ(in0_dst[0], gc::EdgeInputEnd(gc::NodeIndex{0}, gc::InputPort{0}));
    EXPECT_EQ(in0_dst[1], gc::EdgeInputEnd(gc::NodeIndex{1}, gc::InputPort{0}));
    auto in1_dst = group(provided_inputs.destinations, 1);
    EXPECT_EQ(provided_inputs.values[1], uint32_t{500});
    EXPECT_EQ(in1_dst.size(), 2);
    EXPECT_EQ(in1_dst[0], gc::EdgeInputEnd(gc::NodeIndex{0}, gc::InputPort{1}));
    EXPECT_EQ(in1_dst[1], gc::EdgeInputEnd(gc::NodeIndex{1}, gc::InputPort{1}));
    auto in2_dst = group(provided_inputs.destinations, 2);
    EXPECT_EQ(in2_dst.size(), 1);
    EXPECT_EQ(in2_dst[0], gc::EdgeInputEnd(gc::NodeIndex{3}, gc::InputPort{2}));

    // Compute graph
    auto [instr, compiled_source_inputs] = compile(g);
    auto result = gc::ComputationResult{};
    compute(result, g, instr.get(), provided_inputs);

    // Check computation results
    const auto& image =
        group(result.outputs, 3_gc_n)[0_gc_o].as<gc_app::ColorImage>();
    EXPECT_EQ(image.size.width, 600);
    EXPECT_EQ(image.size.height, 500);
    EXPECT_EQ(image.data.size(), 500 * 600);

    EXPECT_EQ(image.data[ 0].v, 0xffffffff);
    EXPECT_EQ(image.data[ 1].v, 0xffffffff);
    EXPECT_EQ(image.data[ 2].v, 0xffffffff);
    EXPECT_EQ(image.data[ 3].v, 0xffffffff);
    EXPECT_EQ(image.data[ 4].v, 0xff000000);
    EXPECT_EQ(image.data[ 5].v, 0xffffffff);
    EXPECT_EQ(image.data[ 6].v, 0xff000000);
    EXPECT_EQ(image.data[ 7].v, 0xffffffff);
    EXPECT_EQ(image.data[ 8].v, 0xff000000);
    EXPECT_EQ(image.data[ 9].v, 0xff000000);
    EXPECT_EQ(image.data[10].v, 0xff000000);
    EXPECT_EQ(image.data[11].v, 0xffffffff);
    EXPECT_EQ(image.data[12].v, 0xff000000);
    EXPECT_EQ(image.data[13].v, 0xffffffff);
    EXPECT_EQ(image.data[14].v, 0xff000000);
    EXPECT_EQ(image.data[15].v, 0xff000000);
    EXPECT_EQ(image.data[16].v, 0xff000000);
    EXPECT_EQ(image.data[17].v, 0xffffffff);
    EXPECT_EQ(image.data[18].v, 0xff000000);
    EXPECT_EQ(image.data[19].v, 0xffffffff);
    EXPECT_EQ(image.data[20].v, 0xff000000);
}
