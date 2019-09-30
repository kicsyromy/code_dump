cmake_minimum_required (VERSION 3.11)

include (cmake/i586-pc-msdosdjgpp.cmake)

set (
    CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

project (racer)

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
add_subdirectory (3rdparty/linalg)
add_subdirectory (3rdparty/pt)
add_subdirectory (3rdparty/sds)
add_subdirectory (3rdparty/stb)
target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    allegro
    linalg
    m
)

target_compile_options (
    ${PROJECT_NAME} PRIVATE
    -fno-exceptions
    -fno-rtti
)

target_compile_definitions (
    ${PROJECT_NAME} PRIVATE
    "SCREEN_WIDTH=320"
    "SCREEN_HEIGHT=200"
)

if (PLATFORM_DJGPP)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -march=i486
        -mtune=i486
    )

    add_custom_target (
        run ALL
        COMMAND "dosbox" ${PROJECT_NAME}.exe
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )

    add_dependencies (
        run ${PROJECT_NAME}
    )
endif ()
