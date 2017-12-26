cmake_minimum_required (VERSION 3.7)
project (vulkan-tutorial CXX)

find_package (Vulkan REQUIRED)
find_package (glfw3 REQUIRED)
find_package (glm REQUIRED)

set (CMAKE_CXX_STANDARD 17)

file (GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.h")
file (GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable (
    ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    glm
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/src/include"
)

