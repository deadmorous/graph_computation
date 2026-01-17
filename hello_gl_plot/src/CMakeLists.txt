cmake_minimum_required(VERSION 3.20)

project(hello_gl_plot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

set(PROJECT_SOURCES
    gl_plot_widget.cpp gl_plot_widget.hpp
    main.cpp
    mainwindow.cpp mainwindow.hpp
)

qt_add_executable(hello_gl_plot
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(hello_gl_plot
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    )

qt_finalize_executable(hello_gl_plot)
