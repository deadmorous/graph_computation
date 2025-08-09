cmake_minimum_required(VERSION 3.20)

project(agc_rt-lib LANGUAGES CXX)

add_library(agc_rt-lib STATIC
    dummy.cpp)

add_library(agc_rt::lib ALIAS agc_rt-lib)



if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(agc_rt-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(agc_rt-lib
        PRIVATE
            -lasan)
endif()



# TODO: Use a small subset of gc::lib
target_link_libraries(agc_rt-lib
    PUBLIC
        gc::lib)

target_include_directories(agc_rt-lib
	PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

# TODO: Remove once lib_config is static
set_target_properties(agc_rt-lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

gc_build_lib_config(agc_rt-lib)
