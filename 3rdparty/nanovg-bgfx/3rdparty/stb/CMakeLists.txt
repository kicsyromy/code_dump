add_library (
    stb-nanovg STATIC
    stb_truetype.cc
)

target_include_directories (
    stb-nanovg SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_compile_options (stb-nanovg PRIVATE -fPIC)
endif ()
