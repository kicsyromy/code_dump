set (COMPONENT_NAME xdg-shell-unstable)

set (CURRENT_DIRECTORY ${CURRENT_DIRECTORY}/xdg-shell-unstable)

add_library (
    ${COMPONENT_NAME} STATIC
    ${CURRENT_DIRECTORY}/xdg-shell-unstable-v6-protocol.c
    ${CURRENT_DIRECTORY}/xdg-shell-unstable-v6-protocol.h
)

add_library (
    ${PROJECT_NAME}::${COMPONENT_NAME} ALIAS
    ${COMPONENT_NAME}
)

target_include_directories (
    ${COMPONENT_NAME} PUBLIC
    ${CURRENT_DIRECTORY}
)
