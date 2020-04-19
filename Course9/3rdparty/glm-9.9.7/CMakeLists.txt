add_library (
    glm INTERFACE
)

target_include_directories (
    glm SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/
)
