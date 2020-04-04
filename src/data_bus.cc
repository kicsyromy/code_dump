#include "data_bus.hh"

#include <cstring>

DataBus::DataBus() noexcept
  : CPU{ *this }
{
    std::memset(RAM.data(), 0, RAM.size());
}

DataBus::~DataBus() noexcept = default;

void DataBus::write(uint16_t address, uint8_t data) noexcept
{
    // if (address >= 0x0000 && address <= 0xFFFF) {
    RAM[address] = data;
    // }
}

uint8_t DataBus::read(uint16_t address, [[maybe_unused]] bool read_only) noexcept
{
    // if (address >= 0x0000 && address <= 0xFFFE) {
    return RAM[address];
    // }
    // return 0x00;
}
