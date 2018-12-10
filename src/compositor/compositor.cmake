set (COMPONENT_NAME compositor)

set (CURRENT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/compositor)

set (
    ${COMPONENT_NAME}_HEADER_FILES
    ${CURRENT_DIRECTORY}/include/harbor_compositor.hh
    ${CURRENT_DIRECTORY}/include/harbor_window_manager.hh
)

set (
    ${COMPONENT_NAME}_SOURCE_FILES
    ${CURRENT_DIRECTORY}/src/harbor_compositor.cc
    ${CURRENT_DIRECTORY}/src/harbor_window_manager.cc
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

target_compile_definitions (
    ${COMPONENT_NAME} PRIVATE
)

target_include_directories (
    ${COMPONENT_NAME} PUBLIC
    ${CURRENT_DIRECTORY}/include
)

target_link_libraries (
    ${COMPONENT_NAME} PUBLIC
    ${PROJECT_NAME}::utilities
    ${PROJECT_NAME}::panel
    3rdparty::miral
    3rdparty::freetype2
)

