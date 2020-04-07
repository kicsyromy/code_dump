#include "cartridge.hh"
#include "cpu6502.hh"
#include "ppu2c02.hh"

Cartridge::Cartridge(Cpu6502 *cpu, Ppu2C02 *ppu) noexcept
  : Device{ &read_data, &write_data }
  , cpu_{ *cpu }
  , ppu_{ *ppu }
{}

void Cartridge::load(std::string_view path) noexcept { static_cast<void>(path); }

std::pair<bool, std::uint8_t> Cartridge::read_data(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const Ppu2C02 *>(instance);
    //    return { true, self->RAM[address & 0x0007] };
    static_cast<void>(address);
    static_cast<void>(self);
    return { false, 0 };
}

bool Cartridge::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ppu2C02 *>(instance);
    //    self->RAM[address & 0x0007] = value;
    static_cast<void>(address);
    static_cast<void>(value);
    static_cast<void>(self);
    return false;
}
