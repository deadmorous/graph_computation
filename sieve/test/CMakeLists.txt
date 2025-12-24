cmake_minimum_required(VERSION 3.20)

project(sieve-test LANGUAGES CXX)

enable_testing()

add_executable(
    sieve-test
    test_sieve_algorithms.cpp
    test_sieve_nodes.cpp)

target_link_libraries(
    sieve-test
    PRIVATE
        GTest::gtest_main
        sieve::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(sieve-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(sieve-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(sieve-test)
