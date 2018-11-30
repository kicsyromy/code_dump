cmake_minimum_required (VERSION 3.11)

project (mir)

include(FindPkgConfig)
pkg_check_modules(MIRAL IMPORTED_TARGET miral REQUIRED)

add_library (miral INTERFACE)
target_link_libraries (miral INTERFACE PkgConfig::MIRAL)
add_library (3rdparty::miral ALIAS miral)
