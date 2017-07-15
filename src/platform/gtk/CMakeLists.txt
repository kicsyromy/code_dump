cmake_minimum_required (VERSION 3.4.0)
project (platform_gtk)
add_definitions (-DPLATFORM_LIB)

find_package (PkgConfig QUIET REQUIRED)
pkg_search_module (GTK3 REQUIRED gtk+-3.0 >= 3.20)

aux_source_directory ("${PROJECT_SOURCE_DIR}" GULP_GTK_SOURCES)
add_library (${PROJECT_NAME} SHARED ${GULP_GTK_SOURCES})
target_include_directories (${PROJECT_NAME} SYSTEM PUBLIC ${GTK3_INCLUDE_DIRS})
target_link_libraries (${PROJECT_NAME} ${GTK3_LIBRARIES})
set_target_properties (
    ${PROJECT_NAME}       PROPERTIES
    LINK_FLAGS            ${GTK3_LDFLAGS}
    CXX_STANDARD          14
    CXX_STANDARD_REQUIRED YES
    VERSION               0.0.1
    SOVERSION             0
)
