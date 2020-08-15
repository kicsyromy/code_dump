function (gn_fetch_gn)
    set (
        oneValueArgs
        ROOT_DIR
    )

    cmake_parse_arguments (VOOT_SKIA "" "${oneValueArgs}" "" "${ARGN}")

    if ((NOT EXISTS "${VOOT_SKIA_ROOT_DIR}/bin/gn") OR (NOT EXISTS "${VOOT_SKIA_ROOT_DIR}/bin/gn.exe"))
        find_program (
            PYTHON python REQUIRED
            NAMES python python2 python3
            DOC "Python executable"
        )

        execute_process (
            COMMAND ${PYTHON} fetch-gn
            WORKING_DIRECTORY ${VOOT_SKIA_ROOT_DIR}/bin
            RESULT_VARIABLE GN_GET_FAILED
            OUTPUT_VARIABLE GN_GET_OUTPUT
            ERROR_VARIABLE GN_GET_OUTPUT
        )

        if (GN_GET_FAILED)
            message(FATAL_ERROR ${GN_GET_OUTPUT})
        endif ()
    endif ()
endfunction ()

function (gn_generate_build_files)
    set (
        oneValueArgs
        ROOT_DIR
    )

    set (
        multiValueArgs
        GN_ARGS
    )

    cmake_parse_arguments (VOOT_SKIA "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    find_program (
        GN gn REQUIRED
        HINTS ${VOOT_SKIA_ROOT_DIR}/bin
        DOC "Skia native build system generator"
    )

    string (REPLACE ";" " " VOOT_SKIA_GN_ARGS "${VOOT_SKIA_GN_ARGS}")
    file (RELATIVE_PATH SKIA_GEN_DIR ${VOOT_SKIA_ROOT_DIR} ${CMAKE_CURRENT_BINARY_DIR})

    execute_process (
        COMMAND ${GN} gen ${SKIA_GEN_DIR} --args=${VOOT_SKIA_GN_ARGS}
        WORKING_DIRECTORY ${VOOT_SKIA_ROOT_DIR}
        RESULT_VARIABLE GENERATE_FAILED
        OUTPUT_VARIABLE GN_OUTPUT
        ERROR_VARIABLE GN_OUTPUT
    )

    if (GENERATE_FAILED)
        message (FATAL_ERROR ${GN_OUTPUT})
    endif ()
endfunction ()

function (gn_generate_target)
    set (
        oneValueArgs
        NAME
        LIB_PREFIX
        LIB_SUFFIX
    )

    cmake_parse_arguments (VOOT_SKIA "" "${oneValueArgs}" "" "${ARGN}")

    find_program (
        NINJA ninja REQUIRED
        DOC "Ninja build system required for Skia"
    )

    if (UNIX AND (NOT APPLE))

        find_program (
            PYTHON2 python2 REQUIRED
            NAMES python2 python2.7
            DOC "Pyhton2 needed for Skia :|"
        )

        execute_process (
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/python2
        )

        execute_process (
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${PYTHON2} ${CMAKE_CURRENT_BINARY_DIR}/python2/python
        )
    endif ()

    add_custom_target (
        ${VOOT_SKIA_NAME}_helper
        ${CMAKE_COMMAND} -E env "PATH=${CMAKE_CURRENT_BINARY_DIR}/python2:$ENV{PATH}" ${NINJA} -C ${CMAKE_CURRENT_BINARY_DIR}
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${VOOT_SKIA_LIB_PREFIX}skia${VOOT_SKIA_LIB_SUFFIX}
    )

    add_library (
        ${VOOT_SKIA_NAME} INTERFACE
    )

    add_dependencies(${VOOT_SKIA_NAME} ${VOOT_SKIA_NAME}_helper)

    target_link_libraries (
        ${VOOT_SKIA_NAME} INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/${VOOT_SKIA_LIB_PREFIX}skia${VOOT_SKIA_LIB_SUFFIX}
    )
endfunction ()
