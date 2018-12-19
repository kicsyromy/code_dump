set (COMPONENT_NAME compositor)

set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/compositor)

set (
    ${COMPONENT_NAME}_HEADER_FILES
    ${CURRENT_DIRECTORY}/include/bindings.h
    ${CURRENT_DIRECTORY}/include/config.h
    ${CURRENT_DIRECTORY}/include/cursor.h
    ${CURRENT_DIRECTORY}/include/desktop.h
    ${CURRENT_DIRECTORY}/include/ini.h
    ${CURRENT_DIRECTORY}/include/input.h
    ${CURRENT_DIRECTORY}/include/keyboard.h
    ${CURRENT_DIRECTORY}/include/layers.h
    ${CURRENT_DIRECTORY}/include/output.h
    ${CURRENT_DIRECTORY}/include/seat.h
    ${CURRENT_DIRECTORY}/include/server.h
    ${CURRENT_DIRECTORY}/include/switch.h
    ${CURRENT_DIRECTORY}/include/text_input.h
    ${CURRENT_DIRECTORY}/include/view.h
    ${CURRENT_DIRECTORY}/include/virtual_keyboard.h
    ${CURRENT_DIRECTORY}/include/xcursor.h
)

set (
    ${COMPONENT_NAME}_SOURCE_FILES
    ${CURRENT_DIRECTORY}/src/bindings.c
    ${CURRENT_DIRECTORY}/src/config.c
    ${CURRENT_DIRECTORY}/src/cursor.c
    ${CURRENT_DIRECTORY}/src/desktop.c
    ${CURRENT_DIRECTORY}/src/ini.c
    ${CURRENT_DIRECTORY}/src/input.c
    ${CURRENT_DIRECTORY}/src/keyboard.c
    ${CURRENT_DIRECTORY}/src/layer_shell.c
    ${CURRENT_DIRECTORY}/src/main.c
    ${CURRENT_DIRECTORY}/src/output.c
    ${CURRENT_DIRECTORY}/src/seat.c
    ${CURRENT_DIRECTORY}/src/switch.c
    ${CURRENT_DIRECTORY}/src/text_input.c
    ${CURRENT_DIRECTORY}/src/virtual_keyboard.c
    ${CURRENT_DIRECTORY}/src/wl_shell.c
    ${CURRENT_DIRECTORY}/src/xdg_shell.c
    ${CURRENT_DIRECTORY}/src/xdg_shell_v6.c
    ${CURRENT_DIRECTORY}/src/xwayland.c
    ${CURRENT_DIRECTORY}/src/test.cc
)

set (${COMPONENT_NAME}_RESOURCES)

add_library (
    ${COMPONENT_NAME} STATIC
    ${${COMPONENT_NAME}_SOURCE_FILES}
    ${${COMPONENT_NAME}_HEADER_FILES}
)

add_library (
    ${PROJECT_NAME}::${COMPONENT_NAME} ALIAS
    ${COMPONENT_NAME}
)

add_dependencies (
    ${COMPONENT_NAME}
    build_with_meson
)

target_compile_definitions (
    ${COMPONENT_NAME} PRIVATE
    -DWLR_USE_UNSTABLE
)

target_include_directories (
    ${COMPONENT_NAME} PUBLIC
    ${CURRENT_DIRECTORY}/include
)

target_link_libraries (
    ${COMPONENT_NAME} PUBLIC
    ${PROJECT_NAME}::utilities
    ${PROJECT_NAME}::panel
    3rdparty::pixman-1
    3rdparty::libinput
    3rdparty::wlroots
    3rdparty::xkbcommon
)

