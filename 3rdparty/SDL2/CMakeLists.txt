add_subdirectory (SDL2-2.0.12)

# Unless this is set SDL2 defines DLL entry point that messes with MSVC when
# linking into a shared library
target_compile_definitions (SDL2-static PRIVATE -DSDL_STATIC_LIB)

# This library is not compiled with -fPIC on Linux, and since it is
# linked into a shared library -fPIC is required
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_compile_options (
        SDL2-static PRIVATE
        -fPIC
    )
endif ()

set (CMAKE_CONFIG_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/SDL2-2.0.12/include/SDL_config.h")
configure_file (
    "SDL2.h.in" "SDL2.h"
    @ONLY
    NEWLINE_STYLE UNIX
)

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

target_link_libraries (
    sdl2 INTERFACE
    SDL2-static
)

add_library (SDL2::SDL2 ALIAS sdl2)
