cmake_minimum_required(VERSION 3.5)

project(agc_perf-lib LANGUAGES CXX)

add_library(agc_perf-lib STATIC
    canvas.cpp
    mandelbrot-fine-graph.cpp
    mandelbrot-fine-graph-2.cpp
    mandelbrot-fine-graph-3.cpp
    mandelbrot-fine-graph-4.cpp
    mandelbrot-fine-graph-5.cpp
    mandelbrot-fine-graph-6.cpp
    mandelbrot-fine-goto-graph.cpp
    mandelbrot-ref.cpp)

add_library(agc_perf::lib ALIAS agc_perf-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(agc_perf-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(agc_perf-lib
        PRIVATE
            -lasan)
endif()



target_link_libraries(agc_perf-lib
    PUBLIC
        gc::lib)

target_include_directories(agc_perf-lib
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(agc_perf-lib
        PUBLIC
            quill::quill)
endif()

