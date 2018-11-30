cmake_minimum_required (VERSION 3.1)

project (nanovg LANGUAGES C)

add_library (
    ${PROJECT_NAME}
    nanovg.c
)

target_include_directories (
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library (3rdparty::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
