add_subdirectory (3rdparty)

set (NANOVG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nanovg)
file (GLOB NANOVG_SOURCES ${NANOVG_SRC_DIR}/*.c ${NANOVG_SRC_DIR}/*.cpp ${NANOVG_SRC_DIR}/*.h)

add_library (
    nanovg-bgfx STATIC
    ${NANOVG_SOURCES}
)

target_include_directories (
    nanovg-bgfx SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions (
    nanovg-bgfx PRIVATE
    "-D_CRT_SECURE_NO_WARNINGS"
    "-D__STDC_FORMAT_MACROS"
)

target_compile_features (
    nanovg-bgfx PRIVATE
    cxx_std_14
)

if (VOOT_BUILD_SHARED)
    set_property (TARGET nanovg-bgfx PROPERTY POSITION_INDEPENDENT_CODE ON)
endif ()

target_link_libraries (
    nanovg-bgfx PUBLIC
    bgfx::bgfx
    stb-nanovg
)

add_library (bgfx::nanovg ALIAS nanovg-bgfx)

