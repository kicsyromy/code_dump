#include "cpu6502.hh"

#include "data_bus.hh"

Cpu6502::Cpu6502(DataBus &bus) noexcept
  : data_bus_{ bus }
{}

Cpu6502::~Cpu6502() noexcept = default;

void Cpu6502::write(std::uint16_t address, std::uint8_t data) noexcept
{
    data_bus_.write(address, data);
}

std::uint8_t Cpu6502::read(std::uint16_t address) const noexcept
{
    return data_bus_.read(address, false);
}
