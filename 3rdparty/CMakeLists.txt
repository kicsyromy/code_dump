## Initial configuration
set (BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples." FORCE)
set (BGFX_INSTALL OFF CACHE BOOL "Create installation target." FORCE)
set (BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Include convenience custom targets." FORCE)

set (SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "Compile with -fno-exceptions. Call abort() on any spdlog exceptions" FORCE)
set (SPDLOG_NO_THREAD_ID ON CACHE BOOL "prevent spdlog from querying the thread id on each log call if thread id is not needed" FORCE)
set (SPDLOG_NO_ATOMIC_LEVELS ON CACHE BOOL "prevent spdlog from using of std::atomic log levels (use only if your code never modifies log levels concurrently" FORCE)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set (SPDLOG_CLOCK_COARSE ON CACHE BOOL "Use the much faster (but much less accurate) CLOCK_REALTIME_COARSE instead of the regular clock," FORCE)
endif()

## Catch2
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Catch2-2.11.3/contrib/")
add_subdirectory (Catch2-2.11.3)

## bgfx.cmake (git master)
add_subdirectory (bgfx)

## SDL2
add_subdirectory (SDL2-2.0.12)

## spdlog
add_subdirectory (spdlog-1.5.0)

## GSL (Core Guidelines)
add_subdirectory (GSL-3.0.0)

## Additional configuration required for proper linking

# Unless this is set SDL2 defines DLL entry point that messes with MSVC when
# linking into a shared library
target_compile_definitions (SDL2-static PRIVATE -DSDL_STATIC_LIB)

# These libraries are not compiled with -fPIC on Linux, and since they are
# linked into a shared library -fPIC is required
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_compile_options (SDL2-static PRIVATE -fPIC)
    target_compile_options (spdlog PRIVATE -fPIC)
endif ()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE PATH "Global project modules" FORCE)
