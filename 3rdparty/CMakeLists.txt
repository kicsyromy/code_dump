## Catch2
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Catch2-2.11.3/contrib/")
add_subdirectory (Catch2-2.11.3)

## fmt
add_subdirectory (fmt-6.1.2)

## spdlog
add_subdirectory (spdlog-1.5.0)

## docopt.cpp (git master)
add_subdirectory (docopt.cpp)

## bgfx.cmake (git master)
add_subdirectory (bgfx)

## nanovg-bgfx (from bgfx git master)
add_subdirectory (nanovg-bgfx)

## nanovg-bgfx (from bgfx git master)
add_subdirectory (imgui-bgfx)

## SDL2
add_subdirectory (SDL2-2.0.12)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE PATH "Global project modules" FORCE)
