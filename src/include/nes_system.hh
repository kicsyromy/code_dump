#pragma once

#include "cpu6502.hh"
#include "ram.hh"
#include "ppu2c02.hh"
#include "data_bus.hh"

class NesSystem
{
public:
    NesSystem() noexcept;

public:
    void reset() noexcept;
    void loadCartridge(std::string_view rom_file_path) noexcept;

private:
    std::uint32_t clock_counter_{ 0 };

private:
    Cpu6502 cpu_;
    Ram system_ram_;
    Cartridge cartridge_;
    Ppu2C02 ppu_;
    DataBus<Cartridge, Cpu6502, Ram, Ppu2C02> main_bus_;
};
