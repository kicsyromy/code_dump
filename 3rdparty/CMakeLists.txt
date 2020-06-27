# Cross-platform, graphics API agnostic rendering library
add_subdirectory (bgfx)

# Antialiased 2D vector drawing library (using bgfx as a backend)
add_subdirectory (nanovg-bgfx)

# Cross-platform hardware abstraction layer for computer multimedia hardware components
add_subdirectory (SDL2)

# Fast C++ logging library
add_subdirectory (spdlog)

## GSL (C++ Core Guidelines helper library)
add_subdirectory (GSL)

## C/C++ a header file designed to smooth over some platform-specific annoyances
add_subdirectory (hedley)
