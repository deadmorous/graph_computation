cmake_minimum_required(VERSION 3.5)

project(gc-lib LANGUAGES CXX)

add_library(gc_app-lib STATIC
    eratosthenes_sieve.cpp
    multiply.cpp
    node_registry.cpp
    rect_view.cpp
    project.cpp
    source_param.cpp
    test_sequence.cpp)

add_library(gc_app::lib ALIAS gc_app-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(gc_app-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(gc_app-lib
        PRIVATE
            -lasan)
endif()



target_link_libraries(gc_app-lib
    PUBLIC
        gc::lib)

target_include_directories(gc_app-lib
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(gc_app-lib
        PUBLIC
            quill::quill)
endif()

