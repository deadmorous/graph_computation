cmake_minimum_required(VERSION 3.5)

add_subdirectory(googletest)

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    add_subdirectory(quill)
endif()

add_subdirectory(yaml-cpp)
