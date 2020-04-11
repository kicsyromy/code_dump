#pragma once

#include "device.hh"
#include "pattern_table.hh"

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
    struct MapperInterface
    {
        std::pair<bool, std::uint16_t> (
            *read_f)(std::uint16_t input_address, const Cartridge &cart, const void *) noexcept;
        std::pair<bool, std::uint16_t> (
            *write_f)(std::uint16_t input_address, const Cartridge &cart, void *) noexcept;
    };

public:
    Cartridge(Cpu6502 *cpu, Ppu2C02 *ppu) noexcept;

public:
    bool load(std::string_view path) noexcept;

private:
    std::tuple<Cartridge::MapperInterface *, void *, std::vector<std::uint8_t> *>
    get_mapper_interface(std::uint16_t address) noexcept;
    std::tuple<const Cartridge::MapperInterface *, const void *, const std::vector<std::uint8_t> *>
    get_mapper_interface(std::uint16_t address) const noexcept;

private:
    static std::pair<bool, std::uint8_t> read_request(std::uint16_t address, const void *) noexcept;
    static bool write_request(std::uint16_t address, std::uint8_t value, void *) noexcept;

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
    MapperInterface mapper_cpu_{ nullptr, nullptr };
    Ppu2C02 &ppu_;
    MapperInterface mapper_ppu_{ nullptr, nullptr };
    PatternTable pattern_table_;

private:
    friend class PatternTable;
    friend class Mapper000;
};
