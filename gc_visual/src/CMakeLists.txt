cmake_minimum_required(VERSION 3.20)

project(gc_visual VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    bitmap_view.cpp             ../include/gc_visual/bitmap_view.hpp
    color_editor_widget.cpp     ../include/gc_visual/color_editor_widget.hpp
    computation_progress_widget.cpp ../include/gc_visual/computation_progress_widget.hpp
    computation_thread.cpp      ../include/gc_visual/computation_thread.hpp
    evolution_controller.cpp    ../include/gc_visual/evolution_controller.hpp
    flatten_type.cpp
    graph_bindings.cpp
    graph_broker.cpp            ../include/gc_visual/graph_broker.hpp
    graph_evolution.cpp
    graph_parameter_editor.cpp  ../include/gc_visual/graph_parameter_editor.hpp
    graph_output_visualizer.cpp ../include/gc_visual/graph_output_visualizer.hpp
    main.cpp
    mainwindow.cpp              ../include/gc_visual/mainwindow.hpp
    parse_graph_binding.cpp
    parse_layout.cpp
    vector_editor_widget.cpp    ../include/gc_visual/vector_editor_widget.hpp
    vector_item_model.cpp       ../include/gc_visual/vector_item_model.hpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gc_visual
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET gc_visual APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(gc_visual SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(gc_visual
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(gc_visual
	PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${GRAPH_COMPUTATION_CXX_ROOT_DIR}/3p/magic_enum/include")

target_link_libraries(gc_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        gc_app::lib)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gc_visual)
endif()
set_target_properties(gc_visual PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS gc_visual
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gc_visual)
endif()
