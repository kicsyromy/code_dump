add_executable (
    graphics_base
                    include/address_modes.hh
    data_bus.cc     include/data_bus.hh
    cpu6502.cc      include/cpu6502.hh
                    include/fps.hh
                    include/instruction.hh
                    include/instruction_set.hh
    main.cc
                    include/register.hh
    renderer.cc     include/renderer.hh
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
    imgui-bgfx
    SDL2-static
)
