cmake_minimum_required(VERSION 3.20)

project(gc_types-lib LANGUAGES CXX)

add_library(gc_types-lib STATIC
    color.cpp
    live_time_series.cpp
    palette.cpp)

add_library(gc_types::lib ALIAS gc_types-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc_types-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc_types-lib
        PRIVATE
            -lasan)
endif()



target_link_libraries(gc_types-lib
    PUBLIC
        gc::lib)

target_include_directories(gc_types-lib
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include")
