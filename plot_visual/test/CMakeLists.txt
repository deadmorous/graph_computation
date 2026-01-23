cmake_minimum_required(VERSION 3.20)

project(plot_visual-lib-test LANGUAGES CXX)

enable_testing()

add_executable(
    plot_visual-lib-test
    test_live_time_series.cpp)

target_link_libraries(
    plot_visual-lib-test
    PRIVATE
        GTest::gtest_main
        plot_visual::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(plot_visual-lib-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(plot_visual-lib-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(plot_visual-lib-test)

