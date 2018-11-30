cmake_minimum_required (VERSION 3.11)

project (freetype2)

include(FindPkgConfig)
pkg_check_modules(FREETYPE2 IMPORTED_TARGET freetype2 REQUIRED)

add_library (freetype2 INTERFACE)
target_link_libraries (freetype2 INTERFACE PkgConfig::FREETYPE2)
add_library (3rdparty::freetype2 ALIAS freetype2)

