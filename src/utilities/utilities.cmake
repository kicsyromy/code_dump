set (COMPONENT_NAME utilities)

set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)

set (
    ${COMPONENT_NAME}_HEADER_FILES
    ${CURRENT_DIRECTORY}/include/harbor_egl_config.hh
    ${CURRENT_DIRECTORY}/include/harbor_logger.hh
    ${CURRENT_DIRECTORY}/include/harbor_egl_surface.hh
    ${CURRENT_DIRECTORY}/include/harbor_utilities.hh
    ${CURRENT_DIRECTORY}/src/include/harbor_egl_config_private.hh
    ${CURRENT_DIRECTORY}/src/include/harbor_egl_surface_private.hh
)

set (
    ${COMPONENT_NAME}_SOURCE_FILES
    ${CURRENT_DIRECTORY}/src/harbor_utilities.cc
    ${CURRENT_DIRECTORY}/src/harbor_egl_config.cc
    ${CURRENT_DIRECTORY}/src/harbor_egl_surface.cc
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
)

