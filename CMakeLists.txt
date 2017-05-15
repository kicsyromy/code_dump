cmake_minimum_required(VERSION 2.8)
project (3dengine)

find_package (PkgConfig QUIET REQUIRED)
pkg_search_module (X11 REQUIRED x11-xcb)
pkg_search_module (CAIRO REQUIRED cairo-xlib)

include_directories (${X11_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS})
aux_source_directory ("${PROJECT_SOURCE_DIR}" SOURCES)
add_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries (${PROJECT_NAME} ${X11_LIBRARIES} ${CAIRO_LIBRARIES})
set_target_properties (
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
