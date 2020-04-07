add_executable (
    graphics_base
                        include/address_modes.hh
    cartridge.cc        include/cartridge.hh
    device.cc           include/device.hh
                        include/data_bus.hh
    cpu6502.cc          include/cpu6502.hh
                        include/fps.hh
                        include/name_table.hh
                        include/instruction.hh
                        include/instruction_set.hh
    main.cc
    nes_system.cc       include/nes_system.hh
                        include/palette_table.hh
    ppu2c02.cc          include/ppu2c02.hh
    ram.cc              include/ram.hh
                        include/register.hh
    renderer.cc         include/renderer.hh
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
