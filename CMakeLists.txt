# CMakeList.txt : CMake project for concord, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.14)

project (concord)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (PkgConfig QUIET REQUIRED)
pkg_search_module (wlroots wlroots REQUIRED IMPORTED_TARGET) 
pkg_search_module (wayland-server wayland-server REQUIRED IMPORTED_TARGET) 
pkg_search_module (xkbcommon xkbcommon REQUIRED IMPORTED_TARGET)
pkg_search_module (wayland-protocols wayland-protocols REQUIRED)
pkg_search_module (wayland-scanner wayland-scanner REQUIRED)

pkg_get_variable (WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
pkg_get_variable (WAYLAND_SCANNER_EXE wayland-scanner wayland_scanner)

file (GLOB_RECURSE ${PROJECT_NAME}_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.*")
file (GLOB_RECURSE ${PROJECT_NAME}_SOURCES "${PROJECT_SOURCE_DIR}/src/*.*")

add_executable (
    ${PROJECT_NAME} 
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
)

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.h
    COMMAND ${WAYLAND_SCANNER_EXE} server-header ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.h
)

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c
    COMMAND ${WAYLAND_SCANNER_EXE} private-code ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c
)

target_sources (
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.h
    ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c
)

target_compile_definitions (
    ${PROJECT_NAME} PUBLIC
    "-DWLR_USE_UNSTABLE"
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries (
    ${PROJECT_NAME} PUBLIC
    PkgConfig::wlroots
    PkgConfig::wayland-server
    PkgConfig::xkbcommon
)
