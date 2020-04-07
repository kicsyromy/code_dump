#pragma once

#include "device.hh"

#include <string_view>
#include <vector>

class Cpu6502;
class Ppu2C02;

class Cartridge : public Device
{
public:
    Cartridge(Cpu6502 *cpu, Ppu2C02 *ppu) noexcept;

public:
    void load(std::string_view path) noexcept;

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
    Cpu6502 &cpu_;
    Ppu2C02 &ppu_;
};
