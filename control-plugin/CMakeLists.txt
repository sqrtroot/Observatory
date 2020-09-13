cmake_minimum_required(VERSION 3.13)
project(control_plugin)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-O3")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

set(STELLARIUM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../stellarium/)
add_library(control_plugin SHARED)
set_target_properties(control_plugin PROPERTIES FOLDER "plugins/control_plugin" AUTOMOC TRUE)
set_target_properties(control_plugin PROPERTIES FOLDER "plugins/control_plugin" AUTOUIC TRUE)
set_target_properties(control_plugin PROPERTIES FOLDER "plugins/control_plugin" AUTORCC TRUE)

target_sources(control_plugin
		PUBLIC
		src/gui/controlPluginSettingsWindow.ui
		src/gui/ControlPluginSettingsWindow.cpp
		src/gui/ControlPluginSettingsWindow.hpp
		src/ControlPlugin.cpp
		src/ControlPlugin.hpp
		src/EncoderStateMachine.hpp
		src/InputThread.cpp
		src/InputThread.hpp
		src/StateMachine.cpp
		src/StateMachine.hpp
		src/gui/DateTimeGui.cpp
		src/gui/DateTimeGui.hpp
		src/control-plugin.qrc
		)

target_include_directories(
        control_plugin
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
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

target_link_libraries(control_plugin Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL gpiod gpiodcxx)
target_compile_definitions(control_plugin PUBLIC QT_DYNAMICPLUGIN)
install(FILES "${CMAKE_BINARY_DIR}/libcontrol_plugin.so" DESTINATION "~/.stellarium/modules/control_plugin")

