set (IMGUI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file (GLOB IMGUI_SOURCES ${IMGUI_SRC_DIR}/*.c ${IMGUI_SRC_DIR}/*.cpp ${IMGUI_SRC_DIR}/*.h)

add_library (
    imgui-bgfx STATIC
    ${IMGUI_SOURCES}
)

target_include_directories (
    imgui-bgfx SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions (
    imgui-bgfx PRIVATE
    "-D_CRT_SECURE_NO_WARNINGS"
    "-D__STDC_FORMAT_MACROS"
)

target_link_libraries (
    imgui-bgfx PUBLIC
    dear-imgui
    bgfx
)
