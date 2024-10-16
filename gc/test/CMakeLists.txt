cmake_minimum_required(VERSION 3.5)

project(gc-lib-test LANGUAGES CXX)

enable_testing()

add_executable(
    gc-lib-test
    main.cpp
    test_gc.cpp
    test_gc_nodes.cpp)

target_link_libraries(
    gc-lib-test
    PRIVATE
        GTest::gtest_main
        gc::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc-lib-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc-lib-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(gc-lib-test)

