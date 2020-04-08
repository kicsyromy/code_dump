#pragma once

#include "cartridge.hh"
#include "data_bus.hh"
#include "device.hh"
#include "name_table.hh"
#include "palette_table.hh"

class Ppu2C02 : public Device
{
public:
    Ppu2C02(Cartridge &cartridge) noexcept;

private:
    static std::pair<bool, std::uint8_t> read_data(std::uint16_t address, const void *) noexcept;
    static bool write_data(std::uint16_t address, std::uint8_t value, void *) noexcept;

private:
    std::uint8_t read(std::uint16_t address) const noexcept;
    void write(std::uint16_t address, std::uint8_t data) noexcept;

private:
    NameTable name_table_;
    PaletteTable palette_table_;
    DataBus<Cartridge, NameTable, PaletteTable> data_bus_;

private:
    friend class Cartridge;
};
