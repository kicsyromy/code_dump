set (BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples." FORCE)
set (BGFX_INSTALL OFF CACHE BOOL "Create installation target." FORCE)
set (BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Include convenience custom targets." FORCE)

add_subdirectory (bgfx)

# This library and its dependencies are not compiled with -fPIC on Linux,
# and since it is linked into a shared library -fPIC is required
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_compile_options (astc PRIVATE -fPIC)
    target_compile_options (astc-codec PRIVATE -fPIC)
    target_compile_options (bgfx PRIVATE -fPIC)
    target_compile_options (bx PRIVATE -fPIC)
    target_compile_options (bimg PRIVATE -fPIC)
endif ()

add_library (bgfx::bgfx ALIAS bgfx)
