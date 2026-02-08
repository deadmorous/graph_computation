cmake_minimum_required(VERSION 3.20)

project(gc-types-test LANGUAGES CXX)

enable_testing()

add_executable(
    gc-types-test
    test_live_time_series.cpp
    test_multi_index.cpp)

target_link_libraries(
    gc-types-test
    PRIVATE
        GTest::gtest_main
        gc_types::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc-types-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc-types-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(gc-types-test)
