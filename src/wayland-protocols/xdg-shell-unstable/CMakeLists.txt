cmake_minimum_required (VERSION 3.1)

project (xdg-shell-unstable LANGUAGES C)

add_library (
    ${PROJECT_NAME}
    xdg-shell-unstable-v6-protocol.c
)

target_include_directories (
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
