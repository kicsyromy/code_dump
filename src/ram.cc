#include "ram.hh"

Ram::Ram() noexcept
  : Device{ &read_data, &write_data }
{}

std::pair<bool, uint8_t> Ram::read_data(std::uint16_t address, const void *instance) noexcept
{
    auto self = static_cast<const Ram *>(instance);
    return { true, self->RAM[address & 0x07FF] };
}

bool Ram::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ram *>(instance);
    self->RAM[address & 0x07FF] = value;
    return true;
}
