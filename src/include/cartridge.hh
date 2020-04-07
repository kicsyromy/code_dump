#pragma once

#include "device.hh"

#include <string_view>
#include <vector>

class Cpu6502;
class Ppu2C02;

class Cartridge : public Device
{
private:
    /* iNES header format */
    struct Header
    {
        char name[4];
        std::uint8_t program_rom_size;
        std::uint8_t character_rom_size;
        std::uint8_t mapper1;
        std::uint8_t mapper2;
        std::uint8_t program_ram_size;
        std::uint8_t tv_system1;
        std::uint8_t tv_system2;
        char unused[5];
    };

public:
    Cartridge(Cpu6502 *cpu, Ppu2C02 *ppu) noexcept;

public:
    bool load(std::string_view path) noexcept;

private:
    static std::pair<bool, std::uint8_t> read_data(std::uint16_t address, const void *) noexcept;
    static bool write_data(std::uint16_t address, std::uint8_t value, void *) noexcept;

private:
    std::vector<std::uint8_t> program_memory_{};
    std::vector<std::uint8_t> character_memory_{};

    std::uint8_t mapper_id_{ 0 };
    std::uint8_t program_banks_{ 0 };
    std::uint8_t character_banks_{ 0 };

private:
    Header header_{};

private:
    Cpu6502 &cpu_;
    Ppu2C02 &ppu_;
};
