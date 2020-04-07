#include "ram.hh"

Ram::Ram() noexcept
  : Device(0, 64 * 1024, &read_data, &write_data)
{}

std::pair<bool, uint8_t> Ram::read_data(std::uint16_t address, const void *instance) noexcept
{
    auto self = static_cast<const Ram *>(instance);
    return { true, self->RAM[address] };
}

bool Ram::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ram *>(instance);
    self->RAM[address] = value;
    return true;
}
