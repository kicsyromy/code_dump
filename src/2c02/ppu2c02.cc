#include "platform_definitions.hh"
#include "ppu2c02.hh"

NameTable::NameTable() noexcept
  : Device{ &read_request, &write_request }
{}

std::pair<bool, std::uint8_t> NameTable::read_request(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const NameTable *>(instance);
    return {
        true,
        self->memory_[0][static_cast<std::size_t>(address - NAME_TABLE_PPU_BUS_ADDRESS.lower_bound)]
    };
}

bool NameTable::write_request(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<NameTable *>(instance);
    self->memory_[0][static_cast<std::size_t>(address - NAME_TABLE_PPU_BUS_ADDRESS.lower_bound)] =
        value;
    return true;
}

PaletteTable::PaletteTable() noexcept
  : Device{ &read_request, &write_request }
{}

std::pair<bool, std::uint8_t> PaletteTable::read_request(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const PaletteTable *>(instance);
    return {
        true,
        self->memory_[static_cast<std::size_t>(address - PALETTE_TABLE_PPU_BUS_ADDRESS.lower_bound)]
    };
}

bool PaletteTable::write_request(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{

    auto self = static_cast<PaletteTable *>(instance);
    self->memory_[static_cast<std::size_t>(address - PALETTE_TABLE_PPU_BUS_ADDRESS.lower_bound)] =
        value;
    return true;
}

Ppu2C02::Ppu2C02(Cartridge &cartridge) noexcept
  : Device{ &read_request, &write_request }
  , data_bus_{ { cartridge, CART_PPU_BUS_ADDRESS.lower_bound, CART_PPU_BUS_ADDRESS.upper_bound },
      { name_table_,
          NAME_TABLE_PPU_BUS_ADDRESS.lower_bound,
          NAME_TABLE_PPU_BUS_ADDRESS.upper_bound },
      { palette_table_,
          PALETTE_TABLE_PPU_BUS_ADDRESS.lower_bound,
          PALETTE_TABLE_PPU_BUS_ADDRESS.upper_bound } }
{}

void Ppu2C02::clock() noexcept
{
    const auto &color = palette[std::size_t(rand() % 64)];
    std::size_t framebuffer_index{ std::size_t(scanline_) * FRAME_WIDTH * 4ul +
                                   std::size_t(cycle_ - 1) * 4ul };
    if (framebuffer_index < framebuffer_.size() - 4)
    {
        framebuffer_[framebuffer_index] = color.r;
        framebuffer_[framebuffer_index + 1] = color.g;
        framebuffer_[framebuffer_index + 2] = color.b;
        framebuffer_[framebuffer_index + 3] = 255;
    }

    ++cycle_;
    if (cycle_ >= 341)
    {
        cycle_ = 0;
        ++scanline_;
        if (scanline_ >= 261)
        {
            scanline_ = -1;
            frame_complete_ = true;
        }
    }
}

std::pair<bool, uint8_t> Ppu2C02::read_request(std::uint16_t address, const void *instance) noexcept
{
    uint8_t data = 0x00;

    auto self = static_cast<const Ppu2C02 *>(instance);
    static_cast<void>(address);
    static_cast<void>(self);

    switch (address)
    {
    case 0x0000: // Control
        break;
    case 0x0001: // Mask
        break;
    case 0x0002: // Status
        break;
    case 0x0003: // OAM Address
        break;
    case 0x0004: // OAM Data
        break;
    case 0x0005: // Scroll
        break;
    case 0x0006: // PPU Address
        break;
    case 0x0007: // PPU Data
        break;
    }

    return { false, data };
}

bool Ppu2C02::write_request(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ppu2C02 *>(instance);
    static_cast<void>(address);
    static_cast<void>(value);
    static_cast<void>(self);

    return false;
    switch (address)
    {
    case 0x0000: // Control
        break;
    case 0x0001: // Mask
        break;
    case 0x0002: // Status
        break;
    case 0x0003: // OAM Address
        break;
    case 0x0004: // OAM Data
        break;
    case 0x0005: // Scroll
        break;
    case 0x0006: // PPU Address
        break;
    case 0x0007: // PPU Data
        break;
    }

    return false;
}

std::uint8_t Ppu2C02::read(std::uint16_t address) const noexcept
{
    address = uint16_t(address & 0x3FFF);
    return bus_read_(address, false, &data_bus_);
}

void Ppu2C02::write(std::uint16_t address, std::uint8_t data) noexcept
{
    address = uint16_t(address & 0x3FFF);
    bus_write_(address, data, &data_bus_);
}
