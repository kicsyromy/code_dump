find_package (PkgConfig QUIET REQUIRED)
pkg_search_module (wayland-protocols wayland-protocols REQUIRED)
pkg_search_module (wayland-scanner wayland-scanner REQUIRED)

pkg_get_variable (WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
pkg_get_variable (WAYLAND_SCANNER_EXE wayland-scanner wayland_scanner)

macro (wayland_generate_protocol)
    set (options)
    set (oneValueArgs INTERFACE TARGET)
    set (multiValueArgs)

    cmake_parse_arguments (WL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    unset (options)
    unset (oneValueArgs)
    unset (multiValueArgs)

    set (WL_PROTOCOL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${WL_TARGET}")
    file (WRITE "${WL_PROTOCOL_OUTPUT}.h" "")
    file (WRITE "${WL_PROTOCOL_OUTPUT}.c" "")

    add_library (${WL_TARGET} STATIC)
    target_sources (
        ${WL_TARGET} PRIVATE
        "${WL_PROTOCOL_OUTPUT}.h"
        "${WL_PROTOCOL_OUTPUT}.c"
    )
    target_include_directories (
        ${WL_TARGET} PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_command (
        TARGET ${WL_TARGET} PRE_BUILD
        COMMAND "${WAYLAND_SCANNER_EXE}" server-header "${WAYLAND_PROTOCOLS_DIR}/${WL_INTERFACE}" "${WL_PROTOCOL_OUTPUT}.h"
    )

    add_custom_command (
        TARGET ${WL_TARGET} PRE_BUILD
        COMMAND "${WAYLAND_SCANNER_EXE}" private-code "${WAYLAND_PROTOCOLS_DIR}/${WL_INTERFACE}" "${WL_PROTOCOL_OUTPUT}.c"
    )

    unset (WL_TARGET)
    unset (WL_INTERFACE)
    unset (WL_PROTOCOL_OUTPUT)
endmacro ()
