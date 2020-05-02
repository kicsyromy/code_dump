option (VOOT_ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option (VOOT_ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if (VOOT_ENABLE_CPPCHECK)
    find_program (CPPCHECK cppcheck)
    if (CPPCHECK)
        set (
            CMAKE_CXX_CPPCHECK ${CPPCHECK}
            --suppress=missingInclude
            --enable=all
            --inconclusive
            -i ${CMAKE_SOURCE_DIR}/imgui/lib
         )
    else ()
        message (SEND_ERROR "[Voot] Cppcheck requested but executable not found")
    endif ()
endif ()

if (VOOT_ENABLE_CLANG_TIDY)
    find_program (CLANGTIDY clang-tidy)
    if (CLANGTIDY)
        set (CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
    else ()
        message (SEND_ERROR "[Voot] clang-tidy requested but executable not found")
    endif ()
endif ()


