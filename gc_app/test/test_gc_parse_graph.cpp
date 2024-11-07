#include "gc_app/node_registry.hpp"
#include "gc/source_param.hpp"
#include "gc_app/image.hpp"
#include "gc_app/type_registry.hpp"

#include "gc/graph_computation.hpp"
#include "gc/yaml/parse_graph.hpp"

#include "yaml-cpp/yaml.h"

#include <gtest/gtest.h>


TEST(GcApp, ParseGraph)
{
    // Graph definition in the YAML format
    constexpr auto* config_text = R"(
nodes:
  - name: img_size
    type: source_param
    init:
      - type: UintSize
        value:
          width: 600
          height: 500

  - name: img_size_w
    type: source_param
    init:
      - type: ValuePath
        value: /width

  - name: img_size_h
    type: source_param
    init:
      - type: ValuePath
        value: /height

  - name: palette
    type: source_param
    init:
      - type: IndexedPalette
        value:
          color_map: [0xffffffff]
          overflow_color: 0xff000000

  - name: pw
    type: project

  - name: ph
    type: project

  - name: seq_size
    type: multiply

  - name: sieve
    type: eratosthenes_sieve

  - name: view
    type: rect_view

edges:
  - [img_size.0,          pw.value]
  - [img_size_w.out_0,    pw.path]
  - [img_size,            ph.value]
  - [img_size_h,          ph.path]
  - [pw.projection,       seq_size.lhs]
  - [ph.projection,       seq_size.rhs]
  - [seq_size.product,    sieve.count]
  - [img_size,            view.size]
  - [sieve.sequence,      view.sequence]
  - [palette,             view.palette]
)";

    // Initialize node registry and type registry
    auto node_registry = gc::node_registry();
    gc_app::populate_node_registry(node_registry);

    auto type_registry = gc::type_registry();
    gc_app::populate_type_registry(type_registry);

    // Parse YAML into a node object; parse graph from that node
    auto config = YAML::Load(config_text);
    auto [g, node_map] =
        gc::yaml::parse_graph(config, node_registry, type_registry);

    // Check number of nodes and edges
    EXPECT_EQ(g.nodes.size(), 9);
    EXPECT_EQ(g.edges.size(), 10);

    // Check that nodes in the graph are in the same order as in
    // the YAML file; check `node_map` (well, the check is made for just one
    // node)
    auto img_size =
        g.nodes.at(0).get();
    EXPECT_EQ(img_size, node_map.at("img_size"));

    // Check parameters passed to some node factories
    EXPECT_EQ(img_size->output_count(), 1);
    gc::ValueVec img_size_inputs( 1 );
    gc::InputParameters::get(img_size)
        ->get_inputs(img_size_inputs);
    auto size =
        img_size_inputs[0].as<gc_app::UintSize>();
    EXPECT_EQ(size.width, 600);
    EXPECT_EQ(size.height, 500);

    // Compute graph
    auto [instr, source_inputs] = compile(g);
    auto result = gc::ComputationResult{};
    compute(result, g, instr.get(), source_inputs);

    // Check computation results
    const auto& image = group(result.outputs,8)[0].as<gc_app::Image>();
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
