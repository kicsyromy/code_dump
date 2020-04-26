add_library (
    stb-nanovg STATIC
    stb_truetype.cc
)

target_include_directories (
    stb-nanovg SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
)
