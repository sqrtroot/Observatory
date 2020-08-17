cmake_minimum_required(VERSION 3.16)
project(control_plugin)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Network REQUIRED)

set(STELLARIUM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../stellarium/)
add_library(control_plugin SHARED)

qt_wrap_ui(control_plugin_uis src/gui/controlPluginSettingsWindow.ui)
target_sources(control_plugin
        PUBLIC
        src/gui/ControlPluginSettingsWindow.cpp
        src/gui/ControlPluginSettingsWindow.hpp
        src/ControlPlugin.cpp
        src/ControlPlugin.hpp
        src/ControlThread.cpp
        src/ControlThread.hpp
        ${control_plugin_uis}
        )

target_include_directories(
        control_plugin
        PUBLIC
        ${CMAKE_BINARY_DIR} #needed for qt
        ${STELLARIUM_SOURCE_DIR}/src
        ${STELLARIUM_SOURCE_DIR}/src/core
        ${STELLARIUM_SOURCE_DIR}/src/core/modules
        ${STELLARIUM_SOURCE_DIR}/src/core/planetsephems
        ${STELLARIUM_SOURCE_DIR}/src/external
        ${STELLARIUM_SOURCE_DIR}/src/external/qtcompress
        ${STELLARIUM_SOURCE_DIR}/src/external/qcustomplot
        ${STELLARIUM_SOURCE_DIR}/src/external/glues_stel/source/
        ${STELLARIUM_SOURCE_DIR}/src/external/glues_stel/source/libtess
        ${STELLARIUM_SOURCE_DIR}/src/gui
        ${STELLARIUM_SOURCE_DIR}/src/scripting
        ${INTL_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/src
)

target_link_libraries(control_plugin Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL)
target_compile_definitions(control_plugin PUBLIC QT_DYNAMICPLUGIN)
set_target_properties(control_plugin PROPERTIES FOLDER "plugins/control_plugin" AUTOMOC TRUE)
install(FILES "${CMAKE_BINARY_DIR}/libcontrol_plugin.so" DESTINATION "~/.stellarium/modules/control_plugin")

