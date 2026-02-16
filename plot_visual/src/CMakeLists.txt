cmake_minimum_required(VERSION 3.20)

project(gc_visual VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

set(PROJECT_SOURCES
    detail/draw_error_message.cpp
    layout.cpp
    opengl/time_series_visualizer.cpp
    opengl/visualizer.cpp
    painter/time_series_histogram_visualizer.cpp
    painter/time_series_visualizer.cpp
    painter/visualizer.cpp
    time_series_renderer.cpp
)

add_library(plot_visual-lib STATIC
    ${PROJECT_SOURCES}
)

target_include_directories(plot_visual-lib
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../include")

target_link_libraries(plot_visual-lib
    PUBLIC
        gc::lib
        gc_types::lib
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

if (GRAPH_COMPUTATION_SANITIZE_ADDRESS)
    target_compile_options(plot_visual-lib
        PRIVATE
            -fsanitize=address)
    target_link_libraries(plot_visual-lib
        PRIVATE
            -lasan)
endif()

add_library(plot_visual::lib ALIAS plot_visual-lib)
