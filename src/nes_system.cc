#include "nes_system.hh"

NesSystem::NesSystem() noexcept
  : cpu_{}
  , system_ram_{}
  , cartridge_{ &cpu_, &ppu_ }
  , ppu_{ cartridge_ }
  , main_bus_{ { cpu_, 0x0000, 0x0000 },
      { system_ram_, 0x0000, 0x1FFF },
      { ppu_, 0x2000, 0x3FFF },
      { cartridge_, 0x4020, 0xFFFF } }
{}

void NesSystem::reset() noexcept
{
    cpu_.reset();
    clock_counter_ = 0;
}

void NesSystem::insertCartridge(Cartridge &&) noexcept {}
