#include "agc_app/activation_node_registry.hpp"

#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/value.hpp"

#include "common/strong_span.hpp"

#include <gtest/gtest.h>


using namespace gc::literals;
using namespace std::string_view_literals;

namespace {

using NodeLabels = common::StrongSpan<std::string_view, gc::NodeIndex>;

auto print_dot(std::ostream& s,
               const gc::ActivationGraph& g,
               NodeLabels node_labels)
    -> void
{
    s << "digraph g {\n";

    assert(node_labels.size() == g.nodes.size());
    for (auto inode : g.nodes.index_range())
    {
        auto* node = g.nodes[inode].get();
        auto label = node_labels[inode];
        s << "  N" << inode
          << " [label=\"" << inode << ": " << label
          << "\\n(" << node->meta().type_name << ")\"]\n";
    }

    s << '\n';

    for (const auto& e : g.edges)
    {
        auto from_port_names = g.nodes.at(e.from.node)->output_names();
        if (!from_port_names.index_range().contains(e.from.port))
            common::throw_("Invalid 'from' port in edge ", e,
                           " - must be less than ",
                           int(from_port_names.size().v));
        auto from_port_name = from_port_names[e.from.port];

        auto to_port_names = g.nodes.at(e.to.node)->input_names();
        if (!to_port_names.index_range().contains(e.to.port))
            common::throw_("Invalid 'to' port in edge ", e,
                           " - must be less than ",
                           int(to_port_names.size().v));
        auto to_port_name = to_port_names[e.to.port];

        s << "  N" << e.from.node << " -> N" << e.to.node
          << " [taillabel=\" " << int(e.from.port.v) << ':' << from_port_name
          << " \" headlabel=\" " << int(e.to.port.v) << ':' << to_port_name
          << " \"]\n";
    }

    s << "}\n";
}

} // anonymous namespace


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

TEST(AgcApp_Graph, GenerateMandelbrot)
{
    auto node_registry = agc_app::activation_node_registry();
    // Context_Source<Grid2dParam> grid_param;
    // Context_Source<Complex> z0;
    // Context_Source<uint64_t> max_iter_count;
    // Context_Source<double> result_scale_factor;
    // Context_Source<double> max_mag2;

    auto grid = node_registry.at("grid_2d")({});
    auto split_grid = node_registry.at("split")(std::vector<gc::Value>{4});
    auto iter_count = node_registry.at("counter")({});
    auto repl_z0 = node_registry.at("replicate")({});
    auto f = node_registry.at("mandelbrot_func")({});
    auto f_iter = node_registry.at("func_iterator")({});
    auto split_iter_val = node_registry.at("split")(std::vector<gc::Value>{3});
    auto repl_iter_val = node_registry.at("replicate")({});
    auto threshold_iter_count = node_registry.at("threshold")({});
    auto iter_val_mag2 = node_registry.at("mag2")({});
    auto threshold_iter_val_mag2 = node_registry.at("threshold")({});
    auto split_iter_count = node_registry.at("split")(std::vector<gc::Value>{2});
    auto repl_iter_count = node_registry.at("replicate")({});
    auto result_scale = node_registry.at("scale")({});
    auto canvas = node_registry.at("canvas")({});

    auto g = gc::ActivationGraph{
        .nodes = {
            grid,                       //  0
            split_grid,                 //  1
            iter_count,                 //  2
            repl_z0,                    //  3
            f,                          //  4
            f_iter,                     //  5
            split_iter_val,             //  6
            repl_iter_val,              //  7
            threshold_iter_count,       //  8
            iter_val_mag2,              //  9
            threshold_iter_val_mag2,    // 10
            split_iter_count,           // 11
            repl_iter_count,            // 12
            result_scale,               // 13
            canvas                      // 14
        },

        .edges = {
            // grid.size -> canvas.size
            { { 0_gc_n, 0_gc_o }, { 14_gc_n, 0_gc_i } },

            // grid.point -> split_grid.in
            { { 0_gc_n, 1_gc_o }, { 1_gc_n, 0_gc_i } },

            // split_grid.out[0] -> iter_count.reset
            { { 1_gc_n, 0_gc_o }, { 2_gc_n, 1_gc_i } },

            // split_grid.out[1] -> repl_z0.trigger
            { { 1_gc_n, 1_gc_o }, { 3_gc_n, 1_gc_i } },

            // split_grid.out[2] -> f.c
            { { 1_gc_n, 2_gc_o }, { 4_gc_n, 0_gc_i } },

            // split_grid.out[3] -> f_iter.next
            { { 1_gc_n, 3_gc_o }, { 5_gc_n, 2_gc_i } },

            // iter_count.count -> split_iter_count.in
            { { 2_gc_n, 0_gc_o }, { 11_gc_n, 0_gc_i } },

            // repl_z0.value -> f_iter.init
            { { 3_gc_n, 0_gc_o }, { 5_gc_n, 0_gc_i } },

            // f.value -> f_iter.value
            { { 4_gc_n, 0_gc_o }, { 5_gc_n, 1_gc_i } },

            // f_iter.arg -> f.z
            { { 5_gc_n, 0_gc_o }, { 4_gc_n, 1_gc_i } },

            // f_iter.value -> split_iter_val.in
            { { 5_gc_n, 1_gc_o }, { 6_gc_n, 0_gc_i } },

            // split_iter_val.out[0] -> repl_iter_val.value
            { { 6_gc_n, 0_gc_o }, { 7_gc_n, 0_gc_i } },

            // split_iter_val.out[1] -> iter_count.next
            { { 6_gc_n, 1_gc_o }, { 2_gc_n, 0_gc_i } },

            // repl_iter_val.value -> iter_val_mag2.value
            { { 7_gc_n, 0_gc_o }, { 9_gc_n, 0_gc_i } },

            // threshold_iter_count.pass -> repl_iter_val.trigger
            { { 8_gc_n, 0_gc_o }, { 7_gc_n, 1_gc_i } },

            // threshold_iter_count.fail -> result_scale.value
            { { 8_gc_n, 1_gc_o }, { 13_gc_n, 1_gc_i } },

            // iter_val_mag2.mag2 -> threshold_iter_val_mag2.value
            { { 9_gc_n, 0_gc_o }, { 10_gc_n, 1_gc_i } },

            // threshold_iter_val_mag2.pass -> f_iter.next
            { { 10_gc_n, 0_gc_o }, { 5_gc_n, 2_gc_i } },

            // threshold_iter_val_mag2.fail -> repl_iter_count.trigger
            { { 10_gc_n, 1_gc_o }, { 12_gc_n, 1_gc_i } },

            // split_iter_count.out[0] -> repl_iter_count.value
            { { 11_gc_n, 0_gc_o }, { 12_gc_n, 0_gc_i } },

            // split_iter_count.out[1] -> threshold_iter_count.value
            { { 11_gc_n, 1_gc_o }, { 8_gc_n, 1_gc_i } },

            // repl_iter_count.value -> result_scale.value
            { { 12_gc_n, 0_gc_o }, { 13_gc_n, 1_gc_i } },

            // result_scale.scaled -> canvas.set_next
            { { 13_gc_n, 0_gc_o }, { 14_gc_n, 2_gc_i } },
        }
    };

    std::cout << "====\n";

    auto node_labels = std::array<std::string_view, 15>{
        "grid",
        "split_grid",
        "iter_count",
        "repl_z0",
        "f",
        "f_iter",
        "split_iter_val",
        "repl_iter_val",
        "threshold_iter_count",
        "iter_val_mag2",
        "threshold_iter_val_mag2",
        "split_iter_count",
        "repl_iter_count",
        "result_scale",
        "canvas"
    };


    print_dot(std::cout, g, node_labels);
    std::cout << "====\n";

    // TODO: Need source types defined in node algos
    auto alg_storage = gc::alg::AlgorithmStorage{};
    auto point_type = alg_storage(gc::alg::Type {
        .name = "std::array<double, 2>",
        .header_file = alg_storage(gc::alg::HeaderFile{
            .name = "array"
        })
    });
    auto count_type = alg_storage(gc::alg::Type {
        .name = "uint64_t",
        .header_file = alg_storage(gc::alg::HeaderFile{
            .name = "cstdint"
        })
    });
    auto double_type = alg_storage(gc::alg::Type {
        .name = "double"
    });
    auto source_types = gc::ActivationGraphSourceTypes{};
    source_types.types.push_back(point_type);
    add_to_last_group(
        source_types.destinations,
        gc::EdgeInputEnd{ 3_gc_n, 0_gc_i });
    next_group(source_types.destinations);

    source_types.types.push_back(count_type);
    add_to_last_group(
        source_types.destinations,
        gc::EdgeInputEnd{ 8_gc_n, 0_gc_i });
    next_group(source_types.destinations);


    source_types.types.push_back(double_type);
    add_to_last_group(
        source_types.destinations,
        gc::EdgeInputEnd{ 10_gc_n, 0_gc_i });
    next_group(source_types.destinations);

    source_types.types.push_back(double_type);
    add_to_last_group(
        source_types.destinations,
        gc::EdgeInputEnd{ 13_gc_n, 0_gc_i });
    next_group(source_types.destinations);

    generate_source(std::cout, g, alg_storage, source_types);
}
