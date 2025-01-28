cmake_minimum_required(VERSION 3.5)

project(gc-lib LANGUAGES CXX)

add_library(gc-lib STATIC
    common/detail/ind.cpp
    expect_n_node_args.cpp
    gc/detail/parse_node_port.cpp
    gc/activation_graph.cpp
    gc/activation_node.cpp
    gc/algorithm.cpp
    gc/computation_node_registry.cpp
    gc/edge.cpp
    gc/graph_computation.cpp
    gc/source_inputs.cpp
    node_port_names.cpp
    parse_simple_value.cpp
    type.cpp
    type_registry.cpp
    value.cpp
    value_path.cpp
    yaml/parse_graph.cpp
    yaml/parse_value.cpp)

add_library(gc::lib ALIAS gc-lib)



set(GRAPH_COMPUTATION_LOG_ENGINE "GC_LOG_QUILL" CACHE STRING
    "Logging engine")

set_property(
    CACHE GRAPH_COMPUTATION_LOG_ENGINE PROPERTY STRINGS
    GC_LOG_NONE
    GC_LOG_QUILL)

target_compile_definitions(gc-lib
    PUBLIC
        -DGRAPH_COMPUTATION_LOG_ENGINE=${GRAPH_COMPUTATION_LOG_ENGINE})


if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc-lib
        PRIVATE
            -lasan)
endif()



target_include_directories(gc-lib
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(gc-lib
        PUBLIC
            quill::quill
            yaml-cpp::yaml-cpp)
endif()
