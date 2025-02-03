#include "agc_app/nodes/linspace.hpp"
#include "agc_app/nodes/printer.hpp"

//#include "gc/activation_node.hpp"
#include "gc/activation_graph.hpp"

#include <gtest/gtest.h>


using namespace agc_app;
using namespace gc::literals;
using namespace std::string_view_literals;


TEST(AgcApp_Graph, GenerateSource)
{
    auto linspace_node = make_linspace({});
    auto printer_node = make_printer({});
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
