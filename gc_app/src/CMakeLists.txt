cmake_minimum_required(VERSION 3.20)

find_package(PNG REQUIRED)

project(gc_app-lib LANGUAGES CXX)

add_library(gc_app-lib STATIC
    computation_node_registry.cpp
    nodes/cell_aut/cell2d.cpp
    nodes/cell_aut/gen_cmap_reader.cpp
    nodes/cell_aut/gen_rule_reader.cpp
    nodes/cell_aut/generate_cmap.cpp
    nodes/cell_aut/generate_rules.cpp
    nodes/cell_aut/life.cpp
    nodes/cell_aut/offset_image.cpp
    nodes/cell_aut/random_image.cpp
    nodes/cell_aut/rule_reader.cpp
    nodes/num/eratosthenes_sieve.cpp
    nodes/num/filter_seq.cpp
    nodes/num/multiply.cpp
    nodes/num/test_sequence.cpp
    nodes/num/waring.cpp
    nodes/num/waring_parallel.cpp
    nodes/util/merge.cpp
    nodes/util/project.cpp
    nodes/util/uint_size.cpp
    nodes/visual/image_colorizer.cpp
    nodes/visual/image_loader.cpp
    nodes/visual/rect_view.cpp
    nodes/visual/spiral_view.cpp
    type_registry.cpp)

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
        gc::lib
        gc_types::lib

    PRIVATE
        PNG::PNG)

target_include_directories(gc_app-lib
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include")
