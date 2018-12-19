cmake_minimum_required (VERSION 3.11)

project (miral)

string (TOUPPER ${PROJECT_NAME} ${PROJECT_NAME}_PKGCONFIG_TARGET)

include(FindPkgConfig)
pkg_check_modules(${${PROJECT_NAME}_PKGCONFIG_TARGET} IMPORTED_TARGET ${PROJECT_NAME} REQUIRED)

add_library (${PROJECT_NAME} INTERFACE)
target_link_libraries (${PROJECT_NAME} INTERFACE PkgConfig::${${PROJECT_NAME}_PKGCONFIG_TARGET})
add_library (3rdparty::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
