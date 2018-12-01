cmake_minimum_required (VERSION 3.11)

project (harbor-compositor LANGUAGES CXX)

add_library (harbor-utilities STATIC IMPORTED)
get_target_property (UTILITIES_INCLUDE_PATH harbor-utilities INTERFACE_INCLUDE_DIRECTORIES)

message (FATAL_ERROR ${UTILITIES_INCLUDE_PATH})

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (
    ${PROJECT_NAME}_HEADER_FILES
    src/include/harbor_window_manager.hh
)

set (
    ${PROJECT_NAME}_SOURCE_FILES
    src/main.cc
    src/harbor_window_manager.cc
)

set (${PROJECT_NAME}_RESOURCES)

add_library (
    ${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_SOURCE_FILES}
    ${${PROJECT_NAME}_HEADER_FILES}
)

target_compile_definitions (
    ${PROJECT_NAME} PRIVATE
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    src/include
)

target_link_libraries (
    ${PROJECT_NAME} INTERFACE 
    harbor-utilities
    3rdparty::miral
    3rdparty::freetype2
)

