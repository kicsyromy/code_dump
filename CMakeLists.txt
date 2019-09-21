cmake_minimum_required (VERSION 3.11)

project (wtst)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB_RECURSE C_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.h")
file (GLOB_RECURSE C_SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")

file (GLOB_RECURSE CXX_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.hh")
file (GLOB_RECURSE CXX_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cc")

add_executable (
    ${PROJECT_NAME}
    ${C_HEADERS}
    ${C_SOURCES}
    ${CXX_HEADERS}
    ${CXX_SOURCES}
)
target_include_directories (
    ${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/src/include"
)

target_compile_definitions (
    ${PROJECT_NAME} PRIVATE
)

add_subdirectory (3rdparty/allegro)
add_subdirectory (3rdparty/linalg)
target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    allegro
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
