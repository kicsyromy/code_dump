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

private:
    struct Mapper
    {
        struct Interface
        {
            using mapper_t = void *;
            using const_mapper_t = const void *;

            std::pair<bool, std::uint16_t> (
                *read_f)(std::uint16_t input_address, const_mapper_t) noexcept;
            std::pair<bool, std::uint16_t> (
                *write_f)(std::uint16_t input_address, mapper_t) noexcept;

            mapper_t mapper_object;
        };

        Mapper(Cartridge &cartridge) noexcept;

        std::pair<bool, std::uint16_t> read(std::uint16_t input_address,
            Interface::const_mapper_t) noexcept;
        std::pair<bool, std::uint16_t> write(std::uint16_t input_address,
            Interface::mapper_t) noexcept;

    private:
        Cartridge &cartridge_;
        Interface impl_;

    private:
        std::uint8_t program_banks_;
        std::uint8_t character_banks_;
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

private:
    friend struct Mapper;
};
