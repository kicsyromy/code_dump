SET (BUILD_UTILS OFF)
add_subdirectory (glew-2.1.0/build/cmake)

add_library (
    gl-glew INTERFACE
)

target_include_directories (
    gl-glew SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/glew-2.1.0/include
)

target_link_libraries (
    gl-glew INTERFACE
    glew_s
)
