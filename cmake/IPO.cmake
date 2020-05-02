option (VOOT_ENABLE_IPO "Enable Iterprocedural Optimization, aka Link Time Optimization (LTO)" OFF)

if (VOOT_ENABLE_IPO)
    include (CheckIPOSupported)
    check_ipo_supported (RESULT VOOT_IPO_SUPPORTED OUTPUT output)
endif ()

function (enable_ipo project_name)
    if (VOOT_ENABLE_IPO)
        if (VOOT_IPO_SUPPORTED)
            set_property (
                TARGET ${project_name}
                PROPERTY INTERPROCEDURAL_OPTIMIZATION ON
            )
        else ()
            message (SEND_ERROR "[VOOT] IPO is not supported: ${output}")
        endif ()
    endif ()
endfunction ()
