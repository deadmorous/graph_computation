cmake_minimum_required(VERSION 3.5)

project(agc_perf-test LANGUAGES CXX)

enable_testing()

add_executable(
    agc_perf-test
    test_mandelbrot.cpp)

target_link_libraries(
    agc_perf-test
    PRIVATE
        GTest::gtest_main
        agc_perf::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(agc_perf-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(agc_perf-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(agc_perf-test)

