cmake_minimum_required(VERSION 3.5)

# mpk_mix: add before anything else so its targets are available to all
# subsequent subdirectories. The SUBDIR is a migration-lab relative path;
# in a standalone checkout this would be a proper submodule at 3p/mpk_mix.
include(${CMAKE_CURRENT_LIST_DIR}/mpk_mix/cmake/mpkmix_deps.cmake)
mpkmix_find_or_add(
    TARGET  mpk_mix::headers
    PACKAGE mpk_mix
    SUBDIR  ${CMAKE_CURRENT_LIST_DIR}/mpk_mix)

# googletest goes before benchmark, which lets the latter know about the former.
mpkmix_find_or_add(
    TARGET  GTest::gtest
    PACKAGE GTest
    SUBDIR  ${CMAKE_CURRENT_LIST_DIR}/googletest)
add_subdirectory(benchmark)

if ("${MPKMIX_LOG_ENGINE}" STREQUAL "MPKMIX_LOG_QUILL")
    add_subdirectory(quill)
endif()

mpkmix_find_or_add(
    TARGET  yaml-cpp::yaml-cpp
    PACKAGE yaml-cpp
    SUBDIR  ${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)
