project (mesa)

set (LIBGL gl)
set (LIBGLU glu)

add_library (
    ${LIBGL} INTERFACE
)

target_include_directories (
    ${LIBGL} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GL
)

target_link_libraries (
    ${LIBGL} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgl.a
)

add_library (
    ${LIBGLU} INTERFACE
)

target_include_directories (
    ${LIBGLU} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GL
)

target_link_libraries (
    ${LIBGLU} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglu.a
)
