set (VOOT_SKIA_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/skia")

option (VOOT_SKIA_USE_SYSTEM_EXPAT "Use system-wide installation of Expat instead of bundled version" OFF)
option (VOOT_SKIA_USE_SYSTEM_HARFBUZZ "Use system-wide installation of HarfBuzz instead of bundled version" OFF)
option (VOOT_SKIA_USE_SYSTEM_ICU "Use system-wide installation of ICU instead of bundled version" OFF)
option (VOOT_SKIA_USE_SYSTEM_JPEG "Use system-wide installation of libjpeg-turbo instead of bundled version" OFF)
option (VOOT_SKIA_USE_SYSTEM_PNG "Use system-wide installation of libpng instead of bundled version" OFF)
option (VOOT_SKIA_USE_SYSTEM_WEBP "Use system-wide installation of WebP instead of bundled version" OFF)
option (VOOT_SKIA_USE_SYSTEM_ZLIB "Use system-wide installation of zlib instead of bundled version" OFF)

add_library (
    skia INTERFACE
)

target_include_directories (
    skia SYSTEM INTERFACE
    ${VOOT_SKIA_ROOT_DIR}
    ${VOOT_SKIA_ROOT_DIR}/include
)

set (
    SKIA_GN_ARGS
    cc=\"${CMAKE_C_COMPILER}\"
    cxx=\"${CMAKE_CXX_COMPILER}\"
    extra_cflags_c=[\"${CMAKE_C_FLAGS}\"]
    extra_cflags_cc=[\"${CMAKE_CXX_FLAGS}\"]
    # This only works in debug builds because line 37 is commented out from ${SKIA_BUILD_ROOT}/gn/BUILDCONFIG.gn
    is_official_build=true
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(
        SKIA_GN_ARGS
        ${SKIA_GN_ARGS}
        is_debug=true
    )
endif ()

include (LinuxConfig.cmake)
include (WindowsConfig.cmake)

include (Dependencies.cmake)

voot_skia_add_dependency (
    PACKAGE_NAME EXPAT
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_EXPAT}
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_expat
    LIB_PREFIX ${VOOT_SKIA_LIB_PREFIX}
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

voot_skia_add_dependency (
    PACKAGE_NAME harfbuzz
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_HARFBUZZ}
    USE_PKG_CONFIG
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_harfbuzz
    LIB_PREFIX ${VOOT_SKIA_LIB_PREFIX}
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

voot_skia_add_dependency (
    PACKAGE_NAME ICU
    PACKAGE_COMPONENTS uc
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_ICU}
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_icu
    LIB_PREFIX ${VOOT_SKIA_LIB_PREFIX}
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

voot_skia_add_dependency (
    PACKAGE_NAME JPEG
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_JPEG}
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_libjpeg_turbo
    LIB_PREFIX $<IF:$<CXX_COMPILER_ID:MSVC>,lib,${VOOT_SKIA_LIB_PREFIX}>
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

voot_skia_add_dependency (
    PACKAGE_NAME PNG
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_PNG}
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_libpng
    LIB_PREFIX $<IF:$<CXX_COMPILER_ID:MSVC>,lib,${VOOT_SKIA_LIB_PREFIX}>
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

voot_skia_add_dependency (
    PACKAGE_NAME libwebp
    PACKAGE_COMPONENTS mux demux
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_WEBP}
    USE_PKG_CONFIG
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_libwebp
    LIB_PREFIX $<IF:$<CXX_COMPILER_ID:MSVC>,lib,${VOOT_SKIA_LIB_PREFIX}>
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

voot_skia_add_dependency (
    PACKAGE_NAME ZLIB
    USE_SYSTEM_LIB ${VOOT_SKIA_USE_SYSTEM_ZLIB}
    TARGET skia
    GN_ARGS_VARIABLE SKIA_GN_ARGS
    GN_ARGUMENT_NAME skia_use_system_zlib
    LIB_PREFIX ${VOOT_SKIA_LIB_PREFIX}
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

include (GNBuild.cmake)

gn_fetch_gn (
    ROOT_DIR ${VOOT_SKIA_ROOT_DIR}
)

gn_generate_build_files (
    ROOT_DIR ${VOOT_SKIA_ROOT_DIR}
    GN_ARGS ${SKIA_GN_ARGS}
)

gn_generate_target (
    NAME gn_skia
    LIB_PREFIX ${VOOT_SKIA_LIB_PREFIX}
    LIB_SUFFIX ${VOOT_SKIA_LIB_SUFFIX}
)

target_link_libraries (
    skia INTERFACE
    gn_skia
)

add_library (google::skia ALIAS skia)
