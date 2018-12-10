set (COMPONENT_NAME utilities)

set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)

set (
    ${COMPONENT_NAME}_HEADER_FILES
    ${CURRENT_DIRECTORY}/include/egl/harbor_egl_config.hh
    ${CURRENT_DIRECTORY}/include/egl/harbor_egl_context.hh
    ${CURRENT_DIRECTORY}/include/egl/harbor_egl_display.hh
    ${CURRENT_DIRECTORY}/include/egl/harbor_egl_surface.hh
    ${CURRENT_DIRECTORY}/include/egl/harbor_egl_types.hh
    ${CURRENT_DIRECTORY}/include/wayland/harbor_wl_registry.hh
    ${CURRENT_DIRECTORY}/include/xdg/harbor_xdg_surface.hh
    ${CURRENT_DIRECTORY}/include/harbor_logger.hh
    ${CURRENT_DIRECTORY}/include/harbor_signal.hh
    ${CURRENT_DIRECTORY}/include/harbor_utilities.hh
)

set (
    ${COMPONENT_NAME}_SOURCE_FILES
    ${CURRENT_DIRECTORY}/src/harbor_utilities.cc
)

add_library (
    ${COMPONENT_NAME} STATIC
    ${${COMPONENT_NAME}_SOURCE_FILES}
    ${${COMPONENT_NAME}_HEADER_FILES}
)

add_library (
    ${PROJECT_NAME}::${COMPONENT_NAME} ALIAS
    ${COMPONENT_NAME}
)

target_compile_definitions (
    ${COMPONENT_NAME} PRIVATE
)

target_include_directories (
    ${COMPONENT_NAME} PUBLIC
    ${CURRENT_DIRECTORY}/include
)

target_include_directories (
    ${COMPONENT_NAME} PRIVATE
    ${CURRENT_DIRECTORY}/src/include
)

target_link_libraries (
    ${COMPONENT_NAME} PUBLIC
    ${PROJECT_NAME}::wayland-protocols
)

