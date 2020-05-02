set (BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples." FORCE)
set (BGFX_INSTALL OFF CACHE BOOL "Create installation target." FORCE)
set (BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Include convenience custom targets." FORCE)

set (OpenGL_GL_PREFERENCE "GLVND" CACHE STRING "Set OpenGL to GL Vendor Neutral library" FORCE)

add_subdirectory (bgfx)

if (VOOT_BUILD_SHARED)
    set_property (TARGET astc PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property (TARGET astc-codec PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property (TARGET bgfx PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property (TARGET bx PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property (TARGET bimg PROPERTY POSITION_INDEPENDENT_CODE ON)
endif ()

add_library (bgfx::bgfx ALIAS bgfx)
