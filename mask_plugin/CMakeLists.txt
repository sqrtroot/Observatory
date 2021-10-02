cmake_minimum_required(VERSION 3.13)
project(mask_plugin)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-O3")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

set(STELLARIUM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../stellarium/)
add_library(mask_plugin SHARED)
set_target_properties(mask_plugin PROPERTIES FOLDER "plugins/mask_plugin" AUTOMOC TRUE)
set_target_properties(mask_plugin PROPERTIES FOLDER "plugins/mask_plugin" AUTOUIC TRUE)
set_target_properties(mask_plugin PROPERTIES FOLDER "plugins/mask_plugin" AUTORCC TRUE)

target_sources(mask_plugin
		PUBLIC
		src/MaskPlugin.cpp
		src/MaskPlugin.hpp
		src/mask-plugin.qrc
		)

target_include_directories(
        mask_plugin
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

target_link_libraries(mask_plugin Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL gpiod gpiodcxx)
target_compile_definitions(mask_plugin PUBLIC QT_DYNAMICPLUGIN)
install(FILES "${CMAKE_BINARY_DIR}/libmask_plugin.so" DESTINATION "~/.stellarium/modules/mask_plugin")

