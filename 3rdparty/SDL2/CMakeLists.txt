option (VOOT_USE_SYSTEM_SDL2 "Use system-wide installation of SDL2 instead of bundled version" OFF)

add_library (
    sdl2 INTERFACE
)

target_include_directories (
    sdl2 SYSTEM INTERFACE
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_compile_definitions(
    sdl2 INTERFACE
    -DUSING_GENERATED_CONFIG_H
    -DSDL_MAIN_HANDLED
)

if (NOT VOOT_USE_SYSTEM_SDL2)
    add_subdirectory (SDL2-2.0.12)

    # Unless this is set SDL2 defines DLL entry point that messes with MSVC when
    # linking into a shared library
    target_compile_definitions (SDL2-static PRIVATE -DSDL_STATIC_LIB)

    if (VOOT_BUILD_SHARED)
        set_property (TARGET SDL2-static PROPERTY POSITION_INDEPENDENT_CODE ON)
    endif ()

    set (CMAKE_CONFIG_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/SDL2-2.0.12/include/SDL_config.h")

    target_link_libraries (
        sdl2 INTERFACE
        SDL2-static
    )
else ()
    find_package (PkgConfig QUIET REQUIRED)
    pkg_search_module (SDL2 REQUIRED IMPORTED_TARGET sdl2)

    set (CMAKE_CONFIG_GENERATED "${SDL2_INCLUDEDIR}/SDL2/SDL_config.h")

    target_link_libraries (
        sdl2 INTERFACE
        PkgConfig::SDL2
    )
endif ()

configure_file (
    "SDL2.h.in" "SDL2.h"
    @ONLY
    NEWLINE_STYLE UNIX
)

add_library (SDL2::SDL2 ALIAS sdl2)
