cmake_minimum_required(VERSION 3.20)

project(gc-lib LANGUAGES CXX)

# Generate default build configuration C++ file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/build/default_config.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/default_config.cpp")

set(GENERATED_CONFIG_TYPE_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/config_vars.cpp")

add_library(gc-lib STATIC
    build/build.cpp
    build/config.cpp
    build/lib_config.cpp
    build/parse_config.cpp
    build/scratch_dir.cpp
    common/detail/ind.cpp
    common/expr_calculator.cpp
    dlib/module.cpp
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
    type_registry.cpp
    yaml/parse_graph.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/default_config.cpp"
    "${GENERATED_CONFIG_TYPE_FILE}")

add_library(gc::lib ALIAS gc-lib)



set(MPKMIX_LOG_ENGINE "MPKMIX_LOG_QUILL" CACHE STRING
    "Logging engine")

set_property(
    CACHE MPKMIX_LOG_ENGINE PROPERTY STRINGS
    MPKMIX_LOG_NONE
    MPKMIX_LOG_QUILL)

target_compile_definitions(gc-lib
    PUBLIC
        -DMPKMIX_LOG_ENGINE=${MPKMIX_LOG_ENGINE})


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

target_link_libraries(gc-lib
    PUBLIC
        mpk_mix::log
        mpk_mix::value
        mpk_mix::serial
        yaml-cpp::yaml-cpp
        dl)

if ("${MPKMIX_LOG_ENGINE}" STREQUAL "MPKMIX_LOG_QUILL")
    target_link_libraries(gc-lib
        PUBLIC
            quill::quill)
endif()

set(GENERATED_CONFIG_TYPE_FILE_COMMAND
    ${CMAKE_CURRENT_LIST_DIR}/build/generate_config_vars_file.sh
    ${GENERATED_CONFIG_TYPE_FILE}
    $<CONFIG>)

add_custom_command(
    OUTPUT "${GENERATED_CONFIG_TYPE_FILE}"
    COMMAND ${GENERATED_CONFIG_TYPE_FILE_COMMAND}
    DEPENDS build/generate_config_vars_file.sh
    VERBATIM)

# Ensure the generated file is built before the library
add_dependencies(gc-lib generate_config_vars_file)

# Add a custom target to enforce dependency resolution
add_custom_target(generate_config_vars_file DEPENDS ${GENERATED_FILE})

gc_build_lib_config(gc-lib)
