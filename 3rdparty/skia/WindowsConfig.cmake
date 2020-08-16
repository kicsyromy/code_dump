if (WIN32)
    set (VOOT_SKIA_LIB_PREFIX "")
    set (VOOT_SKIA_LIB_SUFFIX ".lib")

    target_compile_definitions (
        skia INTERFACE
        -DSK_DIRECT3D
    )

    target_link_libraries (
        skia INTERFACE
        D3D12.lib
        DXGI.lib
    )
endif ()
