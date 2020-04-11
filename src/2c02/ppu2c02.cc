#include "pattern_table.hh"
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

    address = std::uint16_t(address & 0x001F);
    if (address == 0x0010) address = std::uint16_t{ 0x0000 };
    if (address == 0x0014) address = std::uint16_t{ 0x0004 };
    if (address == 0x0018) address = std::uint16_t{ 0x0008 };
    if (address == 0x001C) address = std::uint16_t{ 0x000C };

    return { true, self->memory_[address] };
}

bool PaletteTable::write_request(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<PaletteTable *>(instance);

    address = std::uint16_t(address & 0x001F);
    if (address == 0x0010) address = std::uint16_t{ 0x0000 };
    if (address == 0x0014) address = std::uint16_t{ 0x0004 };
    if (address == 0x0018) address = std::uint16_t{ 0x0008 };
    if (address == 0x001C) address = std::uint16_t{ 0x000C };

    self->memory_[address] = value;
    return true;
}

Ppu2C02::Ppu2C02(Cartridge &cartridge) noexcept
  : Device{ &read_request, &write_request }
  , cartridge_{ cartridge }
  , pattern_table_{ cartridge }
  , data_bus_{ { pattern_table_,
                   PATTERN_TABLE_PPU_BUS_ADDRESS.lower_bound,
                   PATTERN_TABLE_PPU_BUS_ADDRESS.upper_bound },
      { name_table_,
          NAME_TABLE_PPU_BUS_ADDRESS.lower_bound,
          NAME_TABLE_PPU_BUS_ADDRESS.upper_bound },
      { palette_table_,
          PALETTE_TABLE_PPU_BUS_ADDRESS.lower_bound,
          PALETTE_TABLE_PPU_BUS_ADDRESS.upper_bound } }
{}

void Ppu2C02::clock() noexcept
{
    if (frame_complete_) frame_complete_ = false;

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

void Ppu2C02::set_color_from_palette_ram(uint8_t pal,
    uint8_t pixel,
    Ppu2C02::pattern_array_t &pattern_array,
    std::size_t index) const noexcept
{
    auto rgb = palette[data_bus_.read(std::uint16_t(
                           PALETTE_TABLE_PPU_BUS_ADDRESS.lower_bound + pal * 4 + pixel)) &
                       0x3F];
    pattern_array[index] = rgb.r;
    pattern_array[index + 1] = rgb.g;
    pattern_array[index + 2] = rgb.b;
    pattern_array[index + 3] = 255;
}

Ppu2C02::pattern_array_t Ppu2C02::get_pattern_table(std::size_t index, std::uint8_t pal) const
{
    std::array<uint8_t, 128 * 128 * BYTES_PER_PIXEL> result;
    for (std::size_t tile_y = 0; tile_y < PatternTable::SIZE; ++tile_y)
    {
        for (std::size_t tile_x = 0; tile_x < PatternTable::SIZE; ++tile_x)
        {
            for (std::size_t row = 0; row < PatternTable::Tile::HEIGHT; ++row)
            {
                /* http://wiki.nesdev.com/w/index.php/PPU_pattern_tables            */
                /* DCBA98 76543210                                                  */
                /* ---------------                                                  */
                /* 0HRRRR CCCCPTTT                                                  */
                /* |||||| |||||+++- T: Fine Y offset, the row number within a tile  */
                /* |||||| ||||+---- P: Bit plane (0: "lower"; 1: "upper")           */
                /* |||||| ++++----- C: Tile column                                  */
                /* ||++++---------- R: Tile row                                     */
                /* |+-------------- H: Half of sprite table (0: "left"; 1: "right") */
                /* +--------------- 0: Pattern table is at $0000-$1FFF              */
                const auto base_address =
                    std::uint16_t(((index & 0x1) << 13) | ((tile_y & 0xF) << 8) |
                                  ((tile_x & 0xF) << 4) | ((0 & 0x1) << 3) | (row & 0x7));

                auto tile_lsb = data_bus_.read(base_address);
                auto tile_msb = data_bus_.read(base_address | (1 << 3));

                for (std::size_t column = 0; column < PatternTable::Tile::WIDTH; ++column)
                {
                    const auto pixel = std::uint8_t(((tile_msb & 0x01) << 1) | (tile_lsb & 0x01));
                    tile_lsb = std::uint8_t(tile_lsb >> 1);
                    tile_msb = std::uint8_t(tile_msb >> 1);

                    const auto final_pixel_x = tile_x * PatternTable::Tile::WIDTH +
                                               (PatternTable::Tile::WIDTH - column - 1);
                    const auto final_pixel_y = tile_y * PatternTable::Tile::HEIGHT + row;

                    set_color_from_palette_ram(pal,
                        pixel,
                        result,
                        final_pixel_y * 128 * BYTES_PER_PIXEL + final_pixel_x * BYTES_PER_PIXEL);
                }
            }
        }
    }

    return result;
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
