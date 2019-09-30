option (PLATFORM_DJGPP "Compile for 32bit DOS with DJGPP" OFF)

if (PLATFORM_DJGPP)
    set (CMAKE_SYSTEM_NAME Generic)
    set (CMAKE_SYSTEM_PROCESSOR x86)
    set (CMAKE_CROSSCOMPILING 1)
    set (
        CMAKE_CXX_LINK_EXECUTABLE
        "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
    )
    add_compile_definitions ("PLATFORM_DJGPP=1")
endif ()
