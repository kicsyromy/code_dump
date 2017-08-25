cmake_minimum_required (VERSION 3.2)

set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_PROCESSOR x86)
set (CMAKE_CROSSCOMPILING 1)

project (DOSTest)
aux_source_directory ("${PROJECT_SOURCE_DIR}/src" SOURCES)
add_executable (${PROJECT_NAME} ${SOURCES})
set_target_properties (
    ${PROJECT_NAME}     PROPERTIES
    CXX_STANDARD        17
    INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include"
)
