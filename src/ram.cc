#include "ram.hh"

Ram::Ram() noexcept
  : Device{ &read_request, &write_request }
{}

std::pair<bool, uint8_t> Ram::read_request(std::uint16_t address, const void *instance) noexcept
{
    auto self = static_cast<const Ram *>(instance);
    return { true, self->RAM[address & 0x07FF] };
}

bool Ram::write_request(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ram *>(instance);
    self->RAM[address & 0x07FF] = value;
    return true;
}
