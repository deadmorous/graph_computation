cmake_minimum_required(VERSION 3.5)

project(gc-lib LANGUAGES CXX)


# Get the C++ compiler executable
get_filename_component(CMAKE_CXX_COMPILER_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
set(CXX_COMPILER_EXECUTABLE ${CMAKE_CXX_COMPILER})

# Get the linker executable (usually the same as the compiler, but can be different)
get_filename_component(CMAKE_LINKER_PATH ${CMAKE_LINKER} DIRECTORY)
set(LINKER_EXECUTABLE ${CMAKE_LINKER})

# Get compile flags (e.g., CMAKE_CXX_FLAGS, CMAKE_CXX_FLAGS_<CONFIG>)
get_property(COMPILE_FLAGS GLOBAL PROPERTY CMAKE_CXX_FLAGS)
get_property(COMPILE_FLAGS_DEBUG GLOBAL PROPERTY CMAKE_CXX_FLAGS_DEBUG)
get_property(COMPILE_FLAGS_RELEASE GLOBAL PROPERTY CMAKE_CXX_FLAGS_RELEASE)

# Get link flags (e.g., CMAKE_EXE_LINKER_FLAGS, CMAKE_SHARED_LINKER_FLAGS)
get_property(LINK_FLAGS GLOBAL PROPERTY CMAKE_EXE_LINKER_FLAGS)
get_property(LINK_FLAGS_SHARED GLOBAL PROPERTY CMAKE_SHARED_LINKER_FLAGS)

# Create a configuration file with the extracted information
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/build/build_config.yaml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/build_config.yaml")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/build/default_config.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/default_config.cpp")


add_library(gc-lib STATIC
    build/build.cpp
    build/config.cpp
    common/detail/ind.cpp
    expect_n_node_args.cpp
    gc/activation_graph.cpp
    gc/activation_node.cpp
    gc/algorithm.cpp
    gc/alg_known_types.cpp
    gc/computation_node_registry.cpp
    gc/detail/parse_node_port.cpp
    gc/edge.cpp
    gc/generate_dot.cpp
    gc/graph_computation.cpp
    gc/simple_graph_util.cpp
    gc/source_inputs.cpp
    node_port_names.cpp
    parse_simple_value.cpp
    type.cpp
    type_registry.cpp
    value.cpp
    value_path.cpp
    yaml/parse_graph.cpp
    yaml/parse_value.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/default_config.cpp")

add_library(gc::lib ALIAS gc-lib)



set(GRAPH_COM
    build/config.cppPUTATION_LOG_ENGINE "GC_LOG_QUILL" CACHE STRING
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
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include"
        PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(gc-lib
        PUBLIC
            quill::quill
            yaml-cpp::yaml-cpp)
endif()
