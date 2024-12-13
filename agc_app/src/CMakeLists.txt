cmake_minimum_required(VERSION 3.5)

project(agc_app-lib LANGUAGES CXX)

add_library(agc_app-lib STATIC
    nodes/linspace.cpp
    nodes/printer.cpp)

add_library(agc_app::lib ALIAS agc_app-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(agc_app-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(agc_app-lib
        PRIVATE
            -lasan)
endif()



target_link_libraries(agc_app-lib
    PUBLIC
        gc::lib)

target_include_directories(agc_app-lib
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

if ("${GRAPH_COMPUTATION_LOG_ENGINE}" STREQUAL "GC_LOG_QUILL")
    target_link_libraries(agc_app-lib
        PUBLIC
            quill::quill)
endif()

