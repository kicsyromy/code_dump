add_library (
    hedley INTERFACE
)

target_include_directories (
    hedley SYSTEM INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library (hedley::hedley ALIAS hedley)
