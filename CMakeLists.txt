cmake_minimum_required(VERSION 3.5)

enable_testing()

project(pytrunk-cxx-root LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(GRAPH_COMPUTATION_CXX_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

option(GRAPH_COMPUTATION_SANITIZE_ADDRESS "Enable address sanitizer" OFF)

add_subdirectory(gc)
add_subdirectory(gc_app)
add_subdirectory(gc_visual)

add_subdirectory(3p)
