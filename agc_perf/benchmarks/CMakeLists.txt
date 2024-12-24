cmake_minimum_required(VERSION 3.5)

project(agc_perf-benchmarks LANGUAGES CXX)

enable_testing()

add_executable(
    agc_perf-benchmarks
    bm_mandelbrot.cpp)

target_link_libraries(
    agc_perf-benchmarks
    PRIVATE
        benchmark::benchmark
        agc_perf::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(agc_perf-benchmarks
        PRIVATE
            -fsanitize=address)
    target_link_libraries(agc_perf-benchmarks
        PRIVATE
            -lasan)
endif()

