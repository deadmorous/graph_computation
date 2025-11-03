cmake_minimum_required(VERSION 3.20)

project(gc-app-test LANGUAGES CXX)

enable_testing()

add_executable(
    gc-app-test
    main.cpp
    test_gc_app_nodes.cpp
    test_gc_other.cpp
    test_gc_parse_graph.cpp)

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

# Ensure data files are copied to the data subdirectory of gc-app-test
add_custom_command(
    TARGET gc-app-test
    POST_BUILD
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/build_data.sh
            $<TARGET_FILE_DIR:gc-app-test>)

