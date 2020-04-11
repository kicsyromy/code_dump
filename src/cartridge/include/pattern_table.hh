#pragma once

#include "device.hh"
#include "platform_definitions.hh"

#include <array>
#include <bitset>

class Cartridge;

class PatternTable : public Device
{
public:
    static constexpr std::size_t SIZE{ 16 }; // 16 x 16 tiles

public:
    struct Tile
    {
        static constexpr std::size_t SIZE_BYTES{ 16 };
        static constexpr std::size_t WIDTH{ 8 };
        static constexpr std::size_t HEIGHT{ 8 };
    };

public:
    PatternTable(Cartridge &cartridge) noexcept;

private:
    static std::pair<bool, std::uint8_t> read_request(std::uint16_t address, const void *) noexcept;
    static bool write_request(std::uint16_t address, std::uint8_t value, void *) noexcept;

private:
    Cartridge &cartridge_;
    std::array<std::array<std::uint8_t, SIZE * SIZE * Tile::SIZE_BYTES>, 2> memory_;
};
