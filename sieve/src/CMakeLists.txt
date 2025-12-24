cmake_minimum_required(VERSION 3.20)

project(sieve-lib LANGUAGES CXX)

add_library(sieve-lib STATIC
    algorithms/image_metrics.cpp
    nodes/i8_image_metrics.cpp
    types/i8_range.cpp
    types/image_metrics.cpp
    computation_node_registry.cpp
    type_registry.cpp)

add_library(sieve::lib ALIAS sieve-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(sieve-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(sieve-lib
        PRIVATE
            -lasan)
endif()



target_link_libraries(sieve-lib
    PUBLIC
        gc_app::lib)

target_include_directories(sieve-lib
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(sieve-lib
        PUBLIC
            quill::quill)
endif()
