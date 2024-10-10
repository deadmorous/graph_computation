cmake_minimum_required(VERSION 3.5)

project(gc-app-test LANGUAGES CXX)

enable_testing()

add_executable(
    gc-app-test
    main.cpp
    test_gc_app_nodes.cpp
    test_gc_load_graph.cpp)

target_link_libraries(
    gc-app-test
    PRIVATE
        GTest::gtest_main
        gc_app::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc-app-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc-app-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(gc-app-test)

