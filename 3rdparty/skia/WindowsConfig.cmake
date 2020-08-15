if (WIN32)
set (OpenGL_GL_PREFERENCE "GLVND" CACHE STRING "Set OpenGL to GL Vendor Neutral library" FORCE)

find_package (OpenGL REQUIRED)

set (VOOT_SKIA_LIB_PREFIX "")
set (VOOT_SKIA_LIB_SUFFIX ".lib")

target_compile_definitions (
    skia INTERFACE
    -DSK_GL
)

target_link_libraries (
    skia INTERFACE
    OpenGL::OpenGL
)
endif ()
