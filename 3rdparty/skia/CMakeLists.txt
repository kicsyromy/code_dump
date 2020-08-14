set (SKIA_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/skia")

add_library (
    skia INTERFACE
)

target_include_directories (
    skia SYSTEM INTERFACE
    ${SKIA_ROOT_DIR}
    ${SKIA_ROOT_DIR}/include
)

include (LinuxConfig.cmake)

find_program (
    GN gn REQUIRED
    HINTS ${SKIA_ROOT_DIR}/bin
    DOC "Skia native build system generator"
)

set (
    GN_ARGS
    cc=\"${CMAKE_C_COMPILER}\"
    cxx=\"${CMAKE_CXX_COMPILER}\"
    # This only works in debug builds because line 37 is commented out from ${SKIA_BUILD_ROOT}/gn/BUILDCONFIG.gn
    is_official_build=true
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (
        GN_ARGS
        ${GN_ARGS}
        is_debug=true
    )
endif ()

string (REPLACE ";" " " GN_ARGS "${GN_ARGS}")
file (RELATIVE_PATH SKIA_GEN_DIR ${SKIA_ROOT_DIR} ${CMAKE_CURRENT_BINARY_DIR})

execute_process (
    COMMAND ${GN} gen ${SKIA_GEN_DIR} --args=${GN_ARGS}
    WORKING_DIRECTORY ${SKIA_ROOT_DIR}
    RESULT_VARIABLE GENERATE_FAILED
    OUTPUT_VARIABLE GN_OUTPUT
    ERROR_VARIABLE GN_OUTPUT
)

if (GENERATE_FAILED)
    message (FATAL_ERROR ${GN_OUTPUT})
endif ()

find_program (
    NINJA ninja REQUIRED
    DOC "Ninja build system required for skia"
)

add_custom_target (
    skia_gn
    ${CMAKE_COMMAND} -E env ${NINJA} -C ${CMAKE_CURRENT_BINARY_DIR}
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${SKIA_LIB_PREFIX}skia${SKIA_LIB_SUFFIX}
)

target_link_libraries (
    skia INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/${SKIA_LIB_PREFIX}skia${SKIA_LIB_SUFFIX}
)

add_dependencies(skia skia_gn)

add_library (google::skia ALIAS skia)
