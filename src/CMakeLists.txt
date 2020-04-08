add_executable (
    graphics_base

    #-----------------------------2c02 PPU------------------------------------#
                               2c02/include/name_table.hh
                               2c02/include/palette_table.hh
    2c02/ppu2c02.cc            2c02/include/ppu2c02.hh

    #-----------------------------6502 CPU------------------------------------#
                               6502/include/address_modes.hh
    6502/cpu6502.cc            6502/include/cpu6502.hh
                               6502/include/instruction.hh
                               6502/include/instruction_set.hh
                               6502/include/register.hh

    #------------------------Cartridge and mappers----------------------------#
    cartridge/cartridge.cc     cartridge/include/cartridge.hh
    cartridge/mapper_000.cc    cartridge/include/mapper_000.hh

    #------------------------------Main system--------------------------------#
    device.cc                  include/device.hh
                               include/data_bus.hh
    main.cc
    nes_system.cc              include/nes_system.hh
    ram.cc                     include/ram.hh

    renderer.cc                include/renderer.hh
)

target_include_directories (
    graphics_base PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/2c02/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/6502/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/cartridge/include"
)

target_link_libraries (
    graphics_base PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    fmt
    docopt_s
    nanovg-bgfx
    imgui-bgfx
    SDL2-static
)
