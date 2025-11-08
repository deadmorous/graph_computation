/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/throw.hpp"

#include "gc_app/node_registry.hpp"
#include "gc_app/type_registry.hpp"

#include "gc/computation_context.hpp"
#include "gc/computation_node_registry.hpp"
#include "gc/graph_computation.hpp"
#include "gc/yaml/parse_graph.hpp"

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <stdexcept>

auto run(int argc, char* argv[])
    -> void
{
    if (argc != 2)
        common::throw_("Usage: gc_cli gc-file");

    // Initialize node registry and type registry
    auto context = gc::ComputationContext{
        .type_registry = gc::type_registry(),
        .node_registry = gc::computation_node_registry()
    };
    gc_app::populate_node_registry(context.node_registry);
    gc_app::populate_type_registry(context.type_registry);

    // Load graph from the YAML file
    auto config = YAML::LoadFile(argv[1]);

    // Parse graph from the node object.
    auto graph_config = config["graph"];
    auto [g, provided_inputs, node_map, input_names] =
        gc::yaml::parse_graph(graph_config, context);

    auto c = computation(g, provided_inputs);

    auto start_time = std::chrono::steady_clock::now();
    compute(c);
    auto end_time = std::chrono::steady_clock::now();

    auto dt =
        std::chrono::nanoseconds{ end_time - start_time }.count() / 1e9;

    std::cout
        << "Computation finished"
           ", pid: " << getpid()
        << ", time elapsed: " << dt << std::endl;
}

auto main(int argc, char* argv[])
    -> int
{
    try
    {
        run(argc, argv);
        return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
