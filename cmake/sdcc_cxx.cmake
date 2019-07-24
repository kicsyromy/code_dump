macro (cbe_precompile_llvm_sources)
    set (LLVM_CBE_EXECUTABLE "/home/code/workspace/8051playground/cmake/bin/llvm-cbe")
    set (LLVM_CBE_EXECUTABLE_ARGS "-O3" "-thread-model=single")

    set (options)
    set (oneValueArgs OBJECT_LIBRARY_TARGET NEW_TARGET GENERATED_SOURCES)
    set (multiValueArgs)

    cmake_parse_arguments (CBE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    unset (options)
    unset (oneValueArgs)
    unset (multiValueArgs)

    set (C_OUTPUT_SOURCES)
    set (LLVM_COMMANDS)

    get_target_property (TARGET_SOURCES ${CBE_OBJECT_LIBRARY_TARGET} SOURCES)

    foreach (source_file ${TARGET_SOURCES})
        get_filename_component (SOURCE_FILE_NAME "${source_file}" NAME)
        set (C_SOURCE_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE_NAME}.cbe.c")

        list (APPEND C_OUTPUT_SOURCES "${C_SOURCE_FILE_NAME}")
        list (APPEND LLVM_COMMANDS "${LLVM_CBE_EXECUTABLE}" ${LLVM_CBE_EXECUTABLE_ARGS} ${SOURCE_FILE_NAME}.obj -o "${C_SOURCE_FILE_NAME}" &&)

        file (WRITE "${C_SOURCE_FILE_NAME}" "")

        unset (C_SOURCE_FILE_NAME)
    endforeach ()

    list (APPEND LLVM_COMMANDS "/bin/true")

    unset (TARGET_SOURCES)

    get_target_property (TARGET_NAME ${CBE_OBJECT_LIBRARY_TARGET} NAME)
    add_custom_target (
        ${CBE_OBJECT_LIBRARY_TARGET}_GENERATE_C_FILES ALL
        COMMAND ${LLVM_COMMANDS}
        VERBATIM
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TARGET_NAME}.dir"
    )
    unset (TARGET_NAME)

    add_dependencies (
        ${CBE_OBJECT_LIBRARY_TARGET}_GENERATE_C_FILES ${CBE_OBJECT_LIBRARY_TARGET}
    )

    set ("${CBE_GENERATED_SOURCES}" "${C_OUTPUT_SOURCES}")

    add_library (${CBE_NEW_TARGET} INTERFACE)

    add_dependencies (
        ${CBE_NEW_TARGET} ${CBE_OBJECT_LIBRARY_TARGET}_GENERATE_C_FILES
    )

    target_sources (
        ${CBE_NEW_TARGET} INTERFACE
        ${C_OUTPUT_SOURCES}
    )

    get_target_property (CBE_OBJECT_LIBRARY_INCLUDE_DIRECTORIES ${CBE_OBJECT_LIBRARY_TARGET} INCLUDE_DIRECTORIES)
    target_include_directories (
        ${CBE_NEW_TARGET} INTERFACE
        ${CBE_OBJECT_LIBRARY_INCLUDE_DIRECTORIES}
    )
    unset (CBE_OBJECT_LIBRARY_INCLUDE_DIRECTORIES)

    target_compile_definitions (
        ${CBE_NEW_TARGET} INTERFACE
        "-D_MSC_VER"
        "-D__forceinline=inline"
    )

    unset (C_OUTPUT_SOURCES)
    unset (LLVM_COMMANDS)

    unset (LLVM_CBE_EXECUTABLE_ARGS)
    unset (LLVM_CBE_EXECUTABLE)
endmacro ()