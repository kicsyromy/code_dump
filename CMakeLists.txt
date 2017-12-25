cmake_minimum_required (VERSION 3.2)

set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_PROCESSOR x86)
set (CMAKE_CROSSCOMPILING 1)

project (DOSTest)
set (CMAKE_CXX_STANDARD 17)

file (GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file (GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable (
    ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)
target_include_directories (
    ${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

add_custom_command (
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
        exe2coff ${PROJECT_NAME}.exe
    COMMAND
        /usr/bin/cat
            $ENV{DJDIR}/../tools/csdpmi7b/bin/CWSDSTUB.EXE
            ${PROJECT_NAME}
        > ${PROJECT_NAME}.exe
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Patching in extended DOS mode support..."
)

add_custom_target (
    run ALL
    COMMAND /usr/bin/dosbox ${PROJECT_NAME}.exe -c \"mount X $ENV{DJDIR}/../tools\"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
