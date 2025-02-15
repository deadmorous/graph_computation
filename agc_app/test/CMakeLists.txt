cmake_minimum_required(VERSION 3.5)

project(agc-app-test LANGUAGES CXX)

enable_testing()

add_executable(
    agc-app-test
    generated_example.cpp
    generated_mandelbrot.cpp
    main.cpp
    test_agc_app_graph.cpp
    test_agc_app_nodes.cpp)

target_link_libraries(
    agc-app-test
    PRIVATE
        GTest::gtest_main
        agc_app::lib
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(agc-app-test
        PRIVATE
            -fsanitize=address)
    target_link_libraries(agc-app-test
        PRIVATE
            -lasan)
endif()

include(GoogleTest)
gtest_discover_tests(agc-app-test)

