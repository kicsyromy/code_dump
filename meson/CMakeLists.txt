cmake_minimum_required (VERSION 3.11)

project (meson_build_files)

add_custom_target (
    ${PROJECT_NAME} SOURCES
    ${PROJECT_SOURCE_DIR}/../src/compositor/meson.build
    ${PROJECT_SOURCE_DIR}/../src/utilities/meson.build
    ${PROJECT_SOURCE_DIR}/../src/meson.build
    ${PROJECT_SOURCE_DIR}/../src/wayland-protocols/xdg-shell-unstable/meson.build
    ${PROJECT_SOURCE_DIR}/../src/wayland-protocols/meson.build
    ${PROJECT_SOURCE_DIR}/../src/panel/meson.build
    ${PROJECT_SOURCE_DIR}/../meson.build
    ${PROJECT_SOURCE_DIR}/../3rdparty/nanovg/meson.build
    ${PROJECT_SOURCE_DIR}/../3rdparty/meson.build
) 
