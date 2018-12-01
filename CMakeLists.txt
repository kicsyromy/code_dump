cmake_minimum_required (VERSION 3.11)

project (harbor LANGUAGES CXX)

add_subdirectory (3rdparty)

add_subdirectory (src/compositor)
add_subdirectory (src/panel)
add_subdirectory (src/utilities)
add_subdirectory (src/wayland-protocols)

add_executable (
    ${PROJECT_NAME}
    src/main.cc
)

target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    harbor-compositor
    harbor-panel
    3rdparty::miral
    3rdparty::freetype2
)

