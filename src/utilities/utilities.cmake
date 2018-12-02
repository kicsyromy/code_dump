set (COMPONENT_NAME utilities)

set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)

set (
    ${COMPONENT_NAME}_HEADER_FILES
    ${CURRENT_DIRECTORY}/include/harbor_logger.hh
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

target_link_libraries (
    ${COMPONENT_NAME} PUBLIC
)

