cmake_minimum_required(VERSION 3.14)

project (rml CXX)

file (GLOB_RECURSE ${PROJECT_NAME}_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.*")
file (GLOB_RECURSE ${PROJECT_NAME}_SOURCES "${PROJECT_SOURCE_DIR}/src/*.*")

add_executable (
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    src/include
)

target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    stdc++fs
    pthread
)

set_target_properties (
    ${PROJECT_NAME}       PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED YES
    VERSION               0.0.1
    SOVERSION             0
)
