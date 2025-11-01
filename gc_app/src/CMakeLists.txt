cmake_minimum_required(VERSION 3.20)

project(gc_app-lib LANGUAGES CXX)

add_library(gc_app-lib STATIC
    computation_node_registry.cpp
    nodes/cell_aut/life.cpp
    nodes/num/eratosthenes_sieve.cpp
    nodes/num/filter_seq.cpp
    nodes/num/multiply.cpp
    nodes/num/test_sequence.cpp
    nodes/num/waring.cpp
    nodes/num/waring_parallel.cpp
    nodes/util/project.cpp
    nodes/util/uint_size.cpp
    nodes/visual/rect_view.cpp
    nodes/visual/spiral_view.cpp
    type_registry.cpp
    types/color.cpp
    types/palette.cpp)

add_library(gc_app::lib ALIAS gc_app-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc_app-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc_app-lib
        PRIVATE
            -lasan)
endif()



target_link_libraries(gc_app-lib
    PUBLIC
        gc::lib)

target_include_directories(gc_app-lib
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(gc_app-lib
        PUBLIC
            quill::quill)
endif()

