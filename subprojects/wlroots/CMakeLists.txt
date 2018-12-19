cmake_minimum_required (VERSION 3.11)

project (wlroots)

add_library (${PROJECT_NAME} INTERFACE)

target_include_directories (
    ${PROJECT_NAME} INTERFACE
    ${PROJECT_SOURCE_DIR}/wlroots/include
    ${PROJECT_BINARY_DIR}/../../meson/subprojects/wlroots/wlroots/include
    ${PROJECT_BINARY_DIR}/../../meson/subprojects/wlroots/wlroots/protocol
)

target_link_libraries (
    ${PROJECT_NAME} INTERFACE
    ${PROJECT_BINARY_DIR}/../../meson/subprojects/wlroots/wlroots/libwlroots.so
)

add_library (3rdparty::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
