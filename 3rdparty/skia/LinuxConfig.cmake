if (UNIX AND NOT APPLE)

set (OpenGL_GL_PREFERENCE "GLVND" CACHE STRING "Set OpenGL to GL Vendor Neutral library" FORCE)

find_package (OpenGL REQUIRED)
find_package (Fontconfig REQUIRED)
find_package (Freetype REQUIRED)

set (VOOT_SKIA_LIB_PREFIX "lib")
set (VOOT_SKIA_LIB_SUFFIX ".a")

target_compile_definitions (
    skia INTERFACE
    -DSK_GL
)

target_link_libraries (
    skia INTERFACE
    OpenGL::OpenGL
    OpenGL::GLX
    Fontconfig::Fontconfig
    Freetype::Freetype
)

endif ()
