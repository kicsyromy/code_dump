#include "nes_system.hh"

NesSystem::NesSystem() noexcept
  : cpu_{}
  , system_ram_{}
  , cartridge_{ &cpu_, &ppu_ }
  , ppu_{ cartridge_ }
  , main_bus_{ { cartridge_, 0x4020, 0xFFFF },
      { cpu_, 0x0000, 0x0000 },
      { system_ram_, 0x0000, 0x1FFF },
      { ppu_, 0x2000, 0x3FFF } }
{}

void NesSystem::reset() noexcept
{
    cpu_.reset();
    clock_counter_ = 0;
}

void NesSystem::loadCartridge(std::string_view rom_file_path) noexcept
{
    cartridge_.load(rom_file_path);
}
