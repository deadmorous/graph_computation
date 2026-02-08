cmake_minimum_required(VERSION 3.20)

enable_testing()

project(graph_computations LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")

set(GRAPH_COMPUTATION_CXX_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(GRAPH_COMPUTATION_SANITIZE_ADDRESS "Enable address sanitizer" OFF)

add_subdirectory(lib_config)

include(cmake/gc_build_lib_config.cmake)

add_subdirectory(gc)
add_subdirectory(agc_app)
add_subdirectory(agc_app_rt)
add_subdirectory(agc_perf)
add_subdirectory(agc_rt)
add_subdirectory(gc_app)
add_subdirectory(gc_cli)
add_subdirectory(gc_types)
add_subdirectory(gc_visual)
add_subdirectory(plot_visual)
add_subdirectory(hello_gl_plot)
add_subdirectory(sieve)

add_subdirectory(3p)
