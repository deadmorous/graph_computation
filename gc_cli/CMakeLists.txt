cmake_minimum_required(VERSION 3.5)

project(gc-lib LANGUAGES CXX)

add_executable(gc_cli
    main.cpp)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc_cli
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc_cli
        PRIVATE
            -lasan)
endif()

target_link_libraries(gc_cli
    PUBLIC
        gc::lib
        gc_app::lib)

