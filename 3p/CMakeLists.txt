cmake_minimum_required(VERSION 3.5)

# googletest goes before benchmark, which lets the latter know about the former.
add_subdirectory(googletest)
add_subdirectory(benchmark)

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    add_subdirectory(quill)
endif()

add_subdirectory(yaml-cpp)
