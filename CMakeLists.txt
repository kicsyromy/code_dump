cmake_minimum_required (VERSION 3.7)
project (vulkan-tutorial CXX)

find_package (Vulkan REQUIRED)
find_package (glfw3 REQUIRED)
find_package (glm REQUIRED)

find_program (SPIR_V_COMPILER glslangValidator)
if (SPIR_V_COMPILER-NOTFOUND)
    message(FATAL_ERROR "Missing SPIR-V compiler")
endif ()

set (CMAKE_CXX_STANDARD 17)

file (GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.h")
file (GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file (GLOB_RECURSE SHADERS "${PROJECT_SOURCE_DIR}/src/shaders/*.*")

file (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/shaders")

add_executable (
    ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${SAHDERS}
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

target_compile_definitions (
    ${PROJECT_NAME} PRIVATE
    "-DSHADER_PATH=\"${PROJECT_BINARY_DIR}/shaders\""
)

set_target_properties (
   ${PROJECT_NAME}       PROPERTIES
   COMPILE_FLAGS        "-std=c++1z"
)

foreach (SHADER_SRC IN LISTS SHADERS)
    add_custom_command (
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND
            ${SPIR_V_COMPILER} -V ${SHADER_SRC}
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/shaders"
        COMMENT "Compilling ${SHADER_SRC}..."
    )
endforeach ()
