cmake_minimum_required (VERSION 3.11)

project (harbor LANGUAGES C CXX)

add_subdirectory (subprojects)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extra-semi")

include (src/utilities/utilities.cmake)
include (src/wayland-protocols/wayland-protocols.cmake)
include (src/compositor/compositor.cmake)
include (src/panel/panel.cmake)

add_executable (
    ${PROJECT_NAME}
    src/main.cc
)

target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}::compositor
    ${PROJECT_NAME}::panel
    3rdparty::miral
    3rdparty::freetype2
)

add_subdirectory (meson)
