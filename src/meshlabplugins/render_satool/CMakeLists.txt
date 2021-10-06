# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if((CMAKE_SYSTEM_PROCESSOR MATCHES "arm") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64"))
    message("seems Apple M1, cannot use sse or avx")
    add_definitions(-DPROCESSOR_ARM)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -mavx -Wno-non-template-friend")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mavx")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mavx")
endif()

set(SOURCES satool_entry.cpp saDialog.cpp sadelegate.cpp)

set(HEADERS satool_entry.h saDialog.h sadelegate.h)

set(RESOURCES meshlab.qrc)

set(UI saDialog.ui)

add_meshlab_plugin(render_satool ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(render_satool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SACore
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool/plugin/MeshLab
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool ${CMAKE_CURRENT_SOURCE_DIR}/../../../../SATool/build/MeshLab)

target_link_libraries(render_satool PRIVATE OpenGL::GLU satool)

