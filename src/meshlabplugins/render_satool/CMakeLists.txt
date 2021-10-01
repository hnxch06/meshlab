# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES satool_entry.cpp saDialog.cpp)

set(HEADERS satool_entry.h saDialog.h)

set(RESOURCES meshlab.qrc)

set(UI saDialog.ui)

add_meshlab_plugin(render_satool ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(render_satool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool/plugin/MeshLab
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool/build/MeshLab)

target_link_libraries(render_satool PRIVATE OpenGL::GLU satool)

