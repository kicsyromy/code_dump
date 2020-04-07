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
    void insertCartridge(Cartridge &&cartridge) noexcept;

private:
    std::uint32_t clock_counter_{ 0 };

private:
    Cpu6502 cpu_;
    Ram system_ram_;
    Cartridge cartridge_;
    Ppu2C02 ppu_;
    DataBus<Cpu6502, Ram, Ppu2C02, Cartridge> main_bus_;
};
