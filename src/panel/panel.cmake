set (COMPONENT_NAME panel)

set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/panel)

set (
    ${COMPONENT_NAME}_HEADER_FILES
    ${CURRENT_DIRECTORY}/include/harbor_panel.hh
)

set (
    ${COMPONENT_NAME}_SOURCE_FILES
    ${CURRENT_DIRECTORY}/src/harbor_panel.cc
)

set (${COMPONENT_NAME}_RESOURCES)

add_library(
    ${COMPONENT_NAME} STATIC
    ${${COMPONENT_NAME}_SOURCE_FILES}
    ${${COMPONENT_NAME}_HEADER_FILES}
)

add_library(
    ${PROJECT_NAME}::${COMPONENT_NAME} ALIAS
    ${COMPONENT_NAME}
)

target_compile_definitions (
    ${COMPONENT_NAME} PRIVATE
)

target_include_directories (
    ${COMPONENT_NAME} PRIVATE
    ${CURRENT_DIRECTORY}/src/include
)

target_include_directories (
    ${COMPONENT_NAME} PUBLIC
    ${CURRENT_DIRECTORY}/include
)

target_link_libraries (
    ${COMPONENT_NAME} PUBLIC
    ${PROJECT_NAME}::utilities
    ${PROJECT_NAME}::wayland-protocols
    3rdparty::nanovg
    3rdparty::egl
    3rdparty::glesv2
    3rdparty::pthread
    3rdparty::wayland-cursor
    3rdparty::wayland-egl
    3rdparty::wayland-server
)

