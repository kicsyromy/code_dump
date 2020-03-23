add_executable (
    graphics_base
    main.cc
)

target_include_directories (
    graphics_base PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries (
    graphics_base PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    docopt_s
    nanovg-bgfx
    SDL2-static
)
