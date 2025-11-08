/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/activation_node_registry.hpp"

#include "agc_app/nodes/counter.hpp"
#include "agc_app/nodes/grid_2d.hpp"
#include "agc_app/nodes/mandelbrot_func.hpp"

#include "agc_rt/context_util.hpp"

#include "agc_app_rt/types/grid_2d_spec.hpp"
#include "agc_app_rt/types/linspace_spec.hpp"

#include "gc/activation_context.hpp"
#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/alg_known_types.hpp"
#include "gc/generate_dot.hpp"
#include "gc/value.hpp"
#include "gc/yaml/parse_graph.hpp"

#include "build/build.hpp"
#include "build/config.hpp"
#include "build/scratch_dir.hpp"

#include "dlib/module.hpp"
#include "dlib/symbol.hpp"

#include "lib_config/lib_config.hpp"

#include <yaml-cpp/yaml.h>

#include <gtest/gtest.h>

#include <fstream>


using namespace gc::literals;
using namespace std::string_view_literals;

namespace {

template<typename Signature>
auto module_func(const dlib::Module& module, std::string_view name)
    -> Signature*
{
    auto symbol = module.symbol(dlib::SymbolNameView{name});
    return symbol.as<Signature>();
}

} // anonymous namespace

TEST(AgcApp_Graph, GenerateSource)
{
    auto node_registry = agc_app::activation_node_registry();
    auto linspace_node = node_registry.at("linspace")({}, {});
    auto printer_node = node_registry.at("printer")({}, {});
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

TEST(AgcApp_Graph, RunFromYaml)
{
    constexpr auto* example_graph_yaml = R"(
nodes:
  - name: linspace_1
    type: linspace

  - name: printer_1
    type: printer

edges:
  - [linspace_1.sequence, printer_1.value]

inputs:
  - name: img_width
    type: LinSpaceSpec
    value:
      first: 1
      last: 10
      count: 10
    destinations: [linspace_1.spec]
)";

    auto a_context = gc::ActivationContext{
        .type_registry = gc::type_registry(),
        .node_registry = agc_app::activation_node_registry()
    };

    // TODO gc_app::populate_type_registry(type_registry);
    a_context.type_registry.register_value(
        "LinSpaceSpec", gc::type_of<agc_app_rt::LinSpaceSpec>());

    // Parse YAML into a node object; parse graph from that node
    auto config = YAML::Load(example_graph_yaml);
    auto [g, provided_inputs, node_map, input_names] =
        gc::yaml::parse_graph(config, a_context);

    auto alg_storage = gc::alg::AlgorithmStorage{};

    auto source_types =
        gc::ActivationGraphSourceTypes{};

    source_types.types.push_back(common::Zero);
    add_to_last_group(
        source_types.destinations,
        gc::EdgeInputEnd{ 0_gc_n, 0_gc_i });
    next_group(source_types.destinations);

    // ---

    auto scratch_dir = build::ScratchDir{};
    auto path = scratch_dir.path();
    // scratch_dir.detach();   // deBUG

    auto source_path = path / "agc_example.cpp";
    {
        std::ofstream s{source_path};
        ASSERT_TRUE(s.is_open());
        generate_source(s, g, alg_storage, source_types);
    }

    auto output = path / "mandel";
    auto build_config = build::default_config();

    auto libs = build::LibConfigVec{
        build::lib_config("agc_app_rt-lib"),
        build::lib_config("agc_rt-lib"),
    };

    std::cout << "BUILD DIRECTORY: " << path << std::endl;
    build::build(
        build_config,
        output,
        scratch_dir,
        build::InputVec{source_path},
        libs,
        {
            .output_type = build::OutputType::SharedObject
        });

    // ---

    auto module = dlib::Module{ output };

    auto create_context =
        module_func<agc_rt::ContextHandle*()>(module, "create_context");

    auto delete_context =
        module_func<void(agc_rt::ContextHandle*)>(module, "delete_context");

    auto entry_point =
        module_func<void(agc_rt::ContextHandle*)>(module, "entry_point");

    auto set_input_var =
        module_func<void(agc_rt::ContextHandle*, uint64_t, const std::any&)>(
            module, "set_context_input_var");

    auto* context = create_context();

    const auto& destinations = provided_inputs.destinations;
    for (auto index : common::group_indices(destinations))
    {
        const auto& value = provided_inputs.values.at(index);
        for (const auto& to : common::group(destinations, index))
            set_input_var(context, to.compressed(), value.data());
    }

    // ---

    // Call `entry_point`
    entry_point(context);

    delete_context(context);
}

TEST(AgcApp_Graph, GenerateMandelbrot)
{
    // ======== Define example activation graph ========

    auto node_registry = agc_app::activation_node_registry();

    auto grid = node_registry.at("grid_2d")({}, {});
    auto split_grid = node_registry.at("split")(std::vector<gc::Value>{4}, {});
    auto iter_count = node_registry.at("counter")({}, {});
    auto repl_z0 = node_registry.at("replicate")({}, {});
    auto f = node_registry.at("mandelbrot_func")({}, {});
    auto f_iter = node_registry.at("func_iterator")({}, {});
    auto split_iter_val =
        node_registry.at("split")(std::vector<gc::Value>{3}, {});
    auto repl_iter_val = node_registry.at("replicate")({}, {});
    auto threshold_iter_count = node_registry.at("threshold")({}, {});
    auto iter_val_mag2 = node_registry.at("mag2")({}, {});
    auto threshold_iter_val_mag2 = node_registry.at("threshold")({}, {});
    auto split_iter_count =
        node_registry.at("split")(std::vector<gc::Value>{2}, {});
    auto repl_iter_count = node_registry.at("replicate")({}, {});
    auto result_scale = node_registry.at("scale")({}, {});
    auto canvas = node_registry.at("canvas")({}, {});
    auto printer = node_registry.at("printer")({}, {});

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
            canvas,                     // 14
            printer                     // 15
        },

        .edges = {
            // grid.size -> canvas.size
            { { 0_gc_n, 0_gc_o }, { 14_gc_n, 0_gc_i } },

            // grid.point -> split_grid.in
            { { 0_gc_n, 1_gc_o }, { 1_gc_n, 0_gc_i } },

            // grid.end -> canvas.flush
            { { 0_gc_n, 2_gc_o }, { 14_gc_n, 3_gc_i } },

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

            // canvas.canvas -> printer.value
            { { 14_gc_n, 0_gc_o }, { 15_gc_n, 0_gc_i } },

        }
    };

    std::cout << "====\n";

    auto node_labels = std::array<std::string_view, 16>{
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
        "canvas",
        "printer"
    };


    gc::generate_dot(std::cout, g, node_labels);
    std::cout << "====\n";

    auto alg_storage =
        gc::alg::AlgorithmStorage{};

    // auto grid_spec_type =
    //     grid->exported_types(alg_storage).at(agc_app::grid_2d_spec_type);
    auto point_type =
        f->exported_types(alg_storage).at(agc_app::mandelbrot_point_type);
    auto count_type =
        iter_count->exported_types(alg_storage).at(agc_app::counter_type);
    auto double_type =
        gc::alg::well_known_type(gc::alg::double_type, alg_storage);


    auto source_types =
        gc::ActivationGraphSourceTypes{};

    source_types.types.push_back(common::Zero);
    add_to_last_group(
        source_types.destinations,
        gc::EdgeInputEnd{ 0_gc_n, 0_gc_i });
    next_group(source_types.destinations);

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

    auto ignored_sources = gc::EdgeInputEndVec{
        // canvas.set is not used - we are using canvas.set_next instead.
        { 14_gc_n, 1_gc_i }
    };


    // ======== Allocate temporary scratch directory ========

    auto scratch_dir = build::ScratchDir{};
    auto path = scratch_dir.path();
    // scratch_dir.detach();   // deBUG

    // ======== Generate source file implementing graph `g` ========

    auto source_path = path / "mandel.cpp";
    {
        std::ofstream s{source_path};
        ASSERT_TRUE(s.is_open());
        generate_source(s, g, alg_storage, source_types, ignored_sources);
    }


    // ======== Build shared object from the generated source file ========

    auto output = path / "mandel";
    auto build_config = build::default_config();

    auto libs = build::LibConfigVec{
        build::lib_config("agc_app_rt-lib"),
        build::lib_config("agc_rt-lib"),
    };

    std::cout << "BUILD DIRECTORY: " << path << std::endl;
    build::build(
        build_config,
        output,
        scratch_dir,
        build::InputVec{source_path},
        libs,
        {
            .output_type = build::OutputType::SharedObject
        });


    // ======== Load generated shared object and find the entry point ========

    auto module = dlib::Module{ output };

    auto create_context =
        module_func<agc_rt::ContextHandle*()>(module, "create_context");

    auto delete_context =
        module_func<void(agc_rt::ContextHandle*)>(module, "delete_context");

    auto entry_point =
        module_func<void(agc_rt::ContextHandle*)>(module, "entry_point");

    auto grid_var =
        module_func<agc_app_rt::Grid2dSpec*(agc_rt::ContextHandle*)>(
            module, "context_input_var_0_0");

    auto z0_var =
        module_func<std::array<double, 2>*(agc_rt::ContextHandle*)>(
            module, "context_input_var_3_0");

    auto iter_count_var =
        module_func<uint64_t*(agc_rt::ContextHandle*)>(
            module, "context_input_var_8_0");

    auto mag2_threshold_var =
        module_func<double*(agc_rt::ContextHandle*)>(
            module, "context_input_var_10_0");

    auto scale_var =
        module_func<double*(agc_rt::ContextHandle*)>(
            module, "context_input_var_13_0");

    auto* context = create_context();

    uint64_t iter_count_value = 100;

    // Grid
    *grid_var(context) = agc_app_rt::Grid2dSpec{
        .rect = {agc_app_rt::Range<double>{ -2.1, 0.7 },
                 agc_app_rt::Range<double>{ -1.2, 1.2 } },
        .resolution = { 0.1, 0.2 } };

    // z0 for iterations
    *z0_var(context) = { 0., 0. };

    // Iteration count
    *iter_count_var(context) = iter_count_value;

    // Mag2 threshold (to detect iterations divergence)
    *mag2_threshold_var(context) = 1000.;

    // Scale factor for final magnitude of iterated value
    *scale_var(context) = 1. / iter_count_value;

    // Call `entry_point`
    entry_point(context);

    delete_context(context);
}
