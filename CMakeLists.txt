cmake_minimum_required (VERSION 3.14)

project (concord)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include (cmake/wayland.cmake)

find_package (PkgConfig QUIET REQUIRED)
pkg_search_module (wlroots wlroots REQUIRED IMPORTED_TARGET) 
pkg_search_module (wayland-server wayland-server REQUIRED IMPORTED_TARGET) 
pkg_search_module (xkbcommon xkbcommon REQUIRED IMPORTED_TARGET)

file (GLOB_RECURSE ${PROJECT_NAME}_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.*")
file (GLOB_RECURSE ${PROJECT_NAME}_WAYLAND_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/include/wayland/*.*")
file (GLOB_RECURSE ${PROJECT_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

add_executable (
    ${PROJECT_NAME} 
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_WAYLAND_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
)

target_compile_definitions (
    ${PROJECT_NAME} PUBLIC
    "-DWLR_USE_UNSTABLE"
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

wayland_generate_protocol (
    INTERFACE stable/xdg-shell/xdg-shell.xml
    TARGET xdg-shell-protocol
)

target_link_libraries (
    ${PROJECT_NAME} PUBLIC
    PkgConfig::wlroots
    PkgConfig::wayland-server
    PkgConfig::xkbcommon
    xdg-shell-protocol
)
