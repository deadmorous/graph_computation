#include "agc_app/activation_node_registry.hpp"

#include "gc/activation_graph.hpp"
#include "gc/value.hpp"

#include <gtest/gtest.h>


using namespace gc::literals;
using namespace std::string_view_literals;


TEST(AgcApp_Graph, GenerateSource)
{
    auto node_registry = agc_app::activation_node_registry();
    auto linspace_node = node_registry.at("linspace")({});
    auto printer_node = node_registry.at("printer")({});
    auto g = gc::ActivationGraph{
        .nodes = { linspace_node, printer_node },
        .edges = {
            gc::Edge{
                .from = gc::EdgeOutputEnd{
                    .node = 0_gc_n,
                    .port = 0_gc_o },
                .to = gc::EdgeInputEnd{
                    .node = 1_gc_n,
                    .port = 0_gc_i } } } };

    generate_source(std::cout, g);
}
