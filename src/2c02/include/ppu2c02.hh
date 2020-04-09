#pragma once

#include "cartridge.hh"
#include "data_bus.hh"
#include "device.hh"
#include "name_table.hh"
#include "palette_table.hh"

class Ppu2C02 : public Device
{
public:
    struct RGB
    {
        std::array<std::uint8_t, 3> rgb;
        std::uint8_t &r;
        std::uint8_t &g;
        std::uint8_t &b;

        constexpr RGB(std::uint8_t red = 0, std::uint8_t green = 0, std::uint8_t blue = 0) noexcept
          : rgb{ red, green, blue }
          , r{ rgb[0] }
          , g{ rgb[1] }
          , b{ rgb[2] }
        {}
    };
    /* clang-format off */
    std::array<RGB, 64> palette{ {
        { 84, 84, 84 }, { 0, 30, 116 }, { 8, 16, 144 }, { 48, 0, 136 }, { 68, 0, 100 },
        { 92, 0, 48 }, { 84, 4, 0 }, { 60, 24, 0 }, { 32, 42, 0 }, { 8, 58, 0 },
        { 0, 64, 0 }, { 0, 60, 0 }, { 0, 50, 60 }, { 0, 0, 0 }, { 0, 0, 0 },
        { 0, 0, 0 }, { 152, 150, 152 }, { 8, 76, 196 }, { 48, 50, 236 }, { 92, 30, 228 },
        { 136, 20, 176 }, { 160, 20, 100 }, { 152, 34, 32 }, { 120, 60, 0 }, { 84, 90, 0 },
        { 40, 114, 0 }, { 8, 124, 0 }, { 0, 118, 40 }, { 0, 102, 120 }, { 0, 0, 0 },
        { 0, 0, 0 }, { 0, 0, 0 }, { 236, 238, 236 }, { 76, 154, 236 }, { 120, 124, 236 },
        { 176, 98, 236 }, { 228, 84, 236 }, { 236, 88, 180 }, { 236, 106, 100 }, { 212, 136, 32 },
        { 160, 170, 0 }, { 116, 196, 0 }, { 76, 208, 32 }, { 56, 204, 108 }, { 56, 180, 204 },
        { 60, 60, 60 }, { 0, 0, 0 }, { 0, 0, 0 }, { 236, 238, 236 }, { 168, 204, 236 },
        { 188, 188, 236 }, { 212, 178, 236 }, { 236, 174, 236 }, { 236, 174, 212 }, { 236, 180, 176 },
        { 228, 196, 144 }, { 204, 210, 120 }, { 180, 222, 120 }, { 168, 226, 144 }, { 152, 226, 180 },
        { 160, 214, 228 }, { 160, 162, 160 }, { 0, 0, 0 }, { 0, 0, 0 } }
    };
    /* clang-format on */

public:
    Ppu2C02(Cartridge &cartridge) noexcept;

public:
    void clock() noexcept;
    inline bool frame_complete() const noexcept { return frame_complete_; }
    inline const auto &framebuffer() const noexcept { return framebuffer_; }

private:
    static std::pair<bool, std::uint8_t> read_request(std::uint16_t address, const void *) noexcept;
    static bool write_request(std::uint16_t address, std::uint8_t value, void *) noexcept;

private:
    std::uint8_t read(std::uint16_t address) const noexcept;
    void write(std::uint16_t address, std::uint8_t data) noexcept;

private:
    std::int16_t scanline_{ 0 };
    std::int16_t cycle_{ 0 };
    bool frame_complete_{ false };

private:
    std::array<std::array<std::uint8_t, 4096>, 2> pattern_table_;
    static constexpr std::size_t FRAME_WIDTH{ 256 };
    static constexpr std::size_t FRAME_HEIGHT{ 240 };
    std::array<std::uint8_t, FRAME_WIDTH * FRAME_HEIGHT * 4> framebuffer_;

private:
    NameTable name_table_;
    PaletteTable palette_table_;
    DataBus<Cartridge, NameTable, PaletteTable> data_bus_;

private:
    friend class Cartridge;
};
