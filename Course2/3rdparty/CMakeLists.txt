## Catch2
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Catch2-2.11.3/contrib/")
add_subdirectory (Catch2-2.11.3)

## docopt.cpp (git master)
add_subdirectory (docopt.cpp)

## fmt
add_subdirectory (fmt-6.1.2)

## GLEW 2.1.0
add_subdirectory (glew)

## SDL2
add_subdirectory (SDL2-2.0.12)

## spdlog
add_subdirectory (spdlog-1.5.0)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE PATH "Global project modules" FORCE)
