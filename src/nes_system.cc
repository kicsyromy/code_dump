#include "platform_definitions.hh"
#include "nes_system.hh"

NesSystem::NesSystem() noexcept
  : cpu_{}
  , system_ram_{}
  , cartridge_{ &cpu_, &ppu_ }
  , ppu_{ cartridge_ }
  , main_bus_{ { cartridge_, CART_CPU_BUS_ADDRESS.lower_bound, CART_CPU_BUS_ADDRESS.upper_bound },
      { cpu_, 0x0000, 0x0000 },
      { system_ram_, RAM_CPU_BUS_ADDRESS.lower_bound, RAM_CPU_BUS_ADDRESS.upper_bound },
      { ppu_, PPU_CPU_BUS_ADDRESS.lower_bound, PPU_CPU_BUS_ADDRESS.upper_bound } }
{}

void NesSystem::reset() noexcept
{
    cpu_.reset();
    clock_counter_ = 0;
}

void NesSystem::clock() noexcept
{
    ppu_.clock();
    if (clock_counter_ % 3 == 0) { cpu_.clock(); }
    ++clock_counter_;
}

void NesSystem::loadCartridge(std::string_view rom_file_path) noexcept
{
    cartridge_.load(rom_file_path);
}
