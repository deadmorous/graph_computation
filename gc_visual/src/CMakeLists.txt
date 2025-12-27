cmake_minimum_required(VERSION 3.20)

project(gc_visual VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FFMpeg REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    computation_thread.cpp      ../include/gc_visual/computation_thread.hpp
    editors/cell2d_gen_cmap_editor_widget.cpp   ../include/gc_visual/editors/cell2d_gen_cmap_editor_widget.hpp
    editors/cell2d_gen_rule_editor_widget.cpp   ../include/gc_visual/editors/cell2d_gen_rule_editor_widget.hpp
    editors/cell2d_rule_editor_widget.cpp   ../include/gc_visual/editors/cell2d_rule_editor_widget.hpp
    editors/color_editor_widget.cpp         ../include/gc_visual/editors/color_editor_widget.hpp
    editors/file_picker_widget.cpp          ../include/gc_visual/editors/file_picker_widget.hpp
    editors/parameter_editor_widget.cpp     ../include/gc_visual/editors/parameter_editor_widget.hpp
    editors/spin_editor_widget.cpp          ../include/gc_visual/editors/spin_editor_widget.hpp
    editors/vector_editor_widget.cpp        ../include/gc_visual/editors/vector_editor_widget.hpp
    editors/vector_item_model.cpp           ../include/gc_visual/editors/vector_item_model.hpp
    flatten_type.cpp
    graph_bindings.cpp
    graph_broker.cpp            ../include/gc_visual/graph_broker.hpp
    graph_evolution.cpp
    main.cpp
    mainwindow.cpp              ../include/gc_visual/mainwindow.hpp
    parse_graph_binding.cpp
    parse_layout.cpp
    plot/layout.cpp
    video_recorder.cpp          ../include/gc_visual/video_recorder.hpp
    visualizers/image_metrics_visualizer.cpp    ../include/gc_visual/visualizers/image_metrics_visualizer.hpp
    visualizers/image_visualizer.cpp    ../include/gc_visual/visualizers/image_visualizer.hpp
    visualizers/text_visualizer.cpp     ../include/gc_visual/visualizers/text_visualizer.hpp
    visualizers/visualizer_widget.cpp   ../include/gc_visual/visualizers/visualizer_widget.hpp
    widgets/bitmap_view.cpp             ../include/gc_visual/widgets/bitmap_view.hpp
    widgets/cell2d_rule_map_view.cpp    ../include/gc_visual/widgets/cell2d_rule_map_view.hpp
    widgets/computation_progress_widget.cpp ../include/gc_visual/widgets/computation_progress_widget.hpp
    widgets/evolution_controller.cpp    ../include/gc_visual/widgets/evolution_controller.hpp
    widgets/graph_output_visualizer.cpp ../include/gc_visual/widgets/graph_output_visualizer.hpp
    widgets/graph_parameter_editor.cpp  ../include/gc_visual/widgets/graph_parameter_editor.hpp
    widgets/image_metrics_view.cpp      ../include/gc_visual/widgets/image_metrics_view.hpp
    widgets/invalidate_button.cpp       ../include/gc_visual/widgets/invalidate_button.hpp
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
        gc_app::lib
        sieve::lib
        FFMpeg::Encoder
    )

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
