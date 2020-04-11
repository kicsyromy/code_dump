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
    void clock() noexcept;
    void loadCartridge(std::string_view rom_file_path) noexcept;

public:
    const auto &cpu() const noexcept { return cpu_; }
    const auto &ppu() const noexcept { return ppu_; }

public:
    void draw_ram_content(uint16_t offset, int rows, int columns) const noexcept;
    void draw_cpu_state(int width, int height) const noexcept;
    void draw_pattern_tables(std::uint8_t palette) const noexcept;

private:
    std::uint32_t clock_counter_{ 0 };

private:
    Cpu6502 cpu_;
    Ram system_ram_;
    Cartridge cartridge_;
    Ppu2C02 ppu_;
    DataBus<Cartridge, Cpu6502, Ram, Ppu2C02> main_bus_;
};
