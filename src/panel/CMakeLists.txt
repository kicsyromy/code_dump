cmake_minimum_required (VERSION 3.11)

project (harbor-panel LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (
    ${PROJECT_NAME}_HEADER_FILES
    include/harbor_panel.hh
)

set (
    ${PROJECT_NAME}_SOURCE_FILES
    src/harbor_panel.cc
)

set (${PROJECT_NAME}_RESOURCES)

add_library(
    ${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_SOURCE_FILES}
    ${${PROJECT_NAME}_HEADER_FILES}
)

target_compile_definitions (
    ${PROJECT_NAME} PRIVATE
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    src/include
)

target_include_directories (
    ${PROJECT_NAME} PUBLIC
    include
)

target_link_libraries (
    ${PROJECT_NAME} INTERFACE
    3rdparty::nanovg
    3rdparty::wayland-egl
    3rdparty::egl
    3rdparty::glesv2
    3rdparty::pthread
)

