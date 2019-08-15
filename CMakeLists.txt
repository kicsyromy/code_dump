cmake_minimum_required (VERSION 3.14)

project (micro-engine)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (PkgConfig QUIET REQUIRED)
pkg_search_module (sfml sfml-all REQUIRED IMPORTED_TARGET)

file (GLOB_RECURSE ${PROJECT_NAME}_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.*")
file (GLOB_RECURSE ${PROJECT_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

add_executable (
    ${PROJECT_NAME} 
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
)

target_compile_definitions (
    ${PROJECT_NAME} PUBLIC
    -DPROJECT_NAME="${PROJECT_NAME}"
    -DASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

target_link_libraries (
    ${PROJECT_NAME} PUBLIC
    PkgConfig::sfml
    pthread
)
