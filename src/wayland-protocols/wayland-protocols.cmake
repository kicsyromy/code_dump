set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/wayland-protocols)

include (${CURRENT_DIRECTORY}/xdg-shell-unstable/xdg-shell-unstable.cmake)

set (COMPONENT_NAME wayland-protocols)

add_library (${COMPONENT_NAME} INTERFACE)

add_library (
    ${PROJECT_NAME}::${COMPONENT_NAME} ALIAS
    ${COMPONENT_NAME}
)

target_link_libraries (
    ${COMPONENT_NAME} INTERFACE
    ${PROJECT_NAME}::xdg-shell-unstable
)

