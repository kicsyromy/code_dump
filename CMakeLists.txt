cmake_minimum_required (VERSION 3.11)

include (cmake/i586-pc-msdosdjgpp.cmake)

project (wolf3d)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.hh")
file (GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cc")

add_executable (
    ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)
target_include_directories (
    ${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/src/include"
)

add_subdirectory (3rdparty/allegro)
add_subdirectory (3rdparty/mesa)
add_subdirectory (3rdparty/linalg)
target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    allegro
    gl
    glu
    linalg
)

add_custom_target (
    run ALL
    COMMAND "dosbox" ${PROJECT_NAME}.exe
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

add_dependencies (
    run ${PROJECT_NAME}
)
