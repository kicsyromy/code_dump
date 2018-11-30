cmake_minimum_required (VERSION 3.11)

project (harbor LANGUAGES CXX)

add_subdirectory (3rdparty)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (
    ${PROJECT_NAME}_HEADER_FILES
    src/include/harbor_window_manager.hh
    src/include/harbor_logger.hh
)

set (
    ${PROJECT_NAME}_SOURCE_FILES
    src/main.cc
    src/harbor_window_manager.cc
)

set (${PROJECT_NAME}_RESOURCES)

add_executable (
    ${PROJECT_NAME}
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
    ${PROJECT_NAME} PRIVATE
    3rdparty::nanovg
    3rdparty::miral
    3rdparty::freetype2
)

