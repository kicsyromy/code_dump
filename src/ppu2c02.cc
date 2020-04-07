#include "ppu2c02.hh"

static constexpr std::uint16_t NAME_TABLE_BASE_ADDRESS{ 0x3F00 };
static constexpr std::uint16_t PALETTE_TABLE_BASE_ADDRESS{ 0x3F00 };

NameTable::NameTable() noexcept
  : Device{ &read_data, &write_data }
{}

std::pair<bool, std::uint8_t> NameTable::read_data(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const NameTable *>(instance);
    return { true, self->memory_[0][static_cast<std::size_t>(address - NAME_TABLE_BASE_ADDRESS)] };
}

bool NameTable::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{

    auto self = static_cast<NameTable *>(instance);
    self->memory_[0][static_cast<std::size_t>(address - NAME_TABLE_BASE_ADDRESS)] = value;
    return true;
}

PaletteTable::PaletteTable() noexcept
  : Device{ &read_data, &write_data }
{}

std::pair<bool, std::uint8_t> PaletteTable::read_data(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const PaletteTable *>(instance);
    return { true, self->memory_[static_cast<std::size_t>(address - PALETTE_TABLE_BASE_ADDRESS)] };
}

bool PaletteTable::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{

    auto self = static_cast<PaletteTable *>(instance);
    self->memory_[static_cast<std::size_t>(address - PALETTE_TABLE_BASE_ADDRESS)] = value;
    return true;
}

Ppu2C02::Ppu2C02(Cartridge &cartridge) noexcept
  : Device{ &read_data, &write_data }
  , data_bus_{ { cartridge, 0x0000, 0x1FFF },
      { name_table_, NAME_TABLE_BASE_ADDRESS, 0x2FFF },
      { palette_table_, PALETTE_TABLE_BASE_ADDRESS, 0x3FFF } }
{}

std::pair<bool, uint8_t> Ppu2C02::read_data(std::uint16_t address, const void *instance) noexcept
{
    auto self = static_cast<const Ppu2C02 *>(instance);
    //    return { true, self->RAM[address & 0x0007] };
    static_cast<void>(address);
    static_cast<void>(self);
    return { false, 0 };
}

bool Ppu2C02::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ppu2C02 *>(instance);
    //    self->RAM[address & 0x0007] = value;
    static_cast<void>(address);
    static_cast<void>(value);
    static_cast<void>(self);
    return false;
}

std::uint8_t Ppu2C02::read(std::uint16_t address) const noexcept
{
    return bus_read_(address, false, &data_bus_);
}

void Ppu2C02::write(std::uint16_t address, std::uint8_t data) noexcept
{
    bus_write_(address, data, &data_bus_);
}
