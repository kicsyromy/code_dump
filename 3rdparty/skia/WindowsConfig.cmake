if (WIN32)
    set (VOOT_SKIA_LIB_PREFIX "")
    set (VOOT_SKIA_LIB_SUFFIX ".lib")

    target_compile_definitions (
        skia INTERFACE
        -DSK_GL
        -DGL_FRAMEBUFFER_BINDING=0x8CA6
    )

    target_link_libraries (
        skia INTERFACE
        Opengl32.lib
    )
endif ()
