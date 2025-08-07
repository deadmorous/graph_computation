cmake_minimum_required(VERSION 3.20)

project(lib_config LANGUAGES CXX)

# TODO: Make it static
add_library(lib_config SHARED
    lib_config.cpp)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(lib_config
        PRIVATE
            -fsanitize=address)
    target_link_libraries(lib_config
        PRIVATE
            -lasan)
endif()



target_include_directories(lib_config
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include")

target_link_libraries(lib_config
    PRIVATE
        gc::lib)

add_dependencies(lib_config gc::lib)
