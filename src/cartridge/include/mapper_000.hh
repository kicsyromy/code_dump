#pragma once

#include "cartridge.hh"

class Mapper000
{
public:
    Mapper000() noexcept;

    static std::pair<bool, std::uint16_t> cpu_read(std::uint16_t input_address,
        Cartridge &cart,
        const void *cpu) noexcept;
    static std::pair<bool, std::uint16_t> cpu_write(std::uint16_t output_address,
        Cartridge &cart,
        void *cpu) noexcept;
    static std::pair<bool, std::uint16_t> ppu_read(std::uint16_t input_address,
        Cartridge &cart,
        const void *ppu) noexcept;
    static std::pair<bool, std::uint16_t> ppu_write(std::uint16_t output_address,
        Cartridge &cart,
        void *ppu) noexcept;
};
