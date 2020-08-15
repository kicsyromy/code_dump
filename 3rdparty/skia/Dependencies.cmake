function (voot_skia_add_dependency)
    set (
        options
        USE_PKG_CONFIG
    )
    
    set (
        oneValueArgs
        USE_SYSTEM_LIB
        TARGET
        PACKAGE_NAME
        GN_ARGUMENT_NAME
        LIB_PREFIX
        LIB_SUFFIX
    )
    
    set (
        multiValueArgs
        GN_ARGS_VARIABLE
        PACKAGE_COMPONENTS
    )

    cmake_parse_arguments (VOOT_SKIA "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    if (VOOT_SKIA_USE_SYSTEM_LIB)
        if (VOOT_SKIA_USE_PKG_CONFIG)
            find_package (PkgConfig QUIET REQUIRED)
            pkg_search_module (${VOOT_SKIA_PACKAGE_NAME} REQUIRED IMPORTED_TARGET ${VOOT_SKIA_PACKAGE_NAME})
            target_link_libraries (
                ${VOOT_SKIA_TARGET} INTERFACE
                PkgConfig::${VOOT_SKIA_PACKAGE_NAME}
            )
            if (DEFINED VOOT_SKIA_PACKAGE_COMPONENTS)
                foreach (component IN LISTS VOOT_SKIA_PACKAGE_COMPONENTS)
                    pkg_search_module (${VOOT_SKIA_PACKAGE_NAME}${component} REQUIRED IMPORTED_TARGET ${VOOT_SKIA_PACKAGE_NAME}${component})
                    target_link_libraries (
                        ${VOOT_SKIA_TARGET} INTERFACE
                        PkgConfig::${VOOT_SKIA_PACKAGE_NAME}${component}
                    )
                endforeach ()
            endif ()
        else ()
            if (DEFINED VOOT_SKIA_PACKAGE_COMPONENTS)
                find_package (${VOOT_SKIA_PACKAGE_NAME} REQUIRED COMPONENTS ${VOOT_SKIA_PACKAGE_COMPONENTS})
                foreach (component IN LISTS VOOT_SKIA_PACKAGE_COMPONENTS)
                    target_link_libraries (
                        ${VOOT_SKIA_TARGET} INTERFACE
                        ${VOOT_SKIA_PACKAGE_NAME}::${component}
                    )
                endforeach ()
            else ()
                find_package (${VOOT_SKIA_PACKAGE_NAME} REQUIRED)
                target_link_libraries (
                    ${VOOT_SKIA_TARGET} INTERFACE
                    ${VOOT_SKIA_PACKAGE_NAME}::${VOOT_SKIA_PACKAGE_NAME}
                )
            endif ()
            
            
        endif ()
        
        set (
            ${VOOT_SKIA_GN_ARGS_VARIABLE}
            ${${VOOT_SKIA_GN_ARGS_VARIABLE}}
            ${VOOT_SKIA_GN_ARGUMENT_NAME}=true
            PARENT_SCOPE
        )
    else ()
        string (TOLOWER "${VOOT_SKIA_PACKAGE_NAME}" LIB_NAME)
        string (REGEX REPLACE "^(lib)+" "" LIB_NAME ${LIB_NAME})

        target_link_options (
            ${VOOT_SKIA_TARGET} BEFORE INTERFACE
            ${CMAKE_CURRENT_BINARY_DIR}/${VOOT_SKIA_LIB_PREFIX}${LIB_NAME}${VOOT_SKIA_LIB_SUFFIX}
        )

        set (
            ${VOOT_SKIA_GN_ARGS_VARIABLE}
            ${${VOOT_SKIA_GN_ARGS_VARIABLE}}
            ${VOOT_SKIA_GN_ARGUMENT_NAME}=false
            PARENT_SCOPE
        )
    endif ()
endfunction ()
