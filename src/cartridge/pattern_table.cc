#include "cartridge.hh"
#include "pattern_table.hh"

PatternTable::PatternTable(Cartridge &cartridge) noexcept
  : Device{ &read_request, &write_request }
  , cartridge_{ cartridge }
{}

std::pair<bool, uint8_t> PatternTable::read_request(std::uint16_t address,
    const void *instance) noexcept
{
    const auto *self = static_cast<const PatternTable *>(instance);
    return Cartridge::read_request(address, &self->cartridge_);
}

bool PatternTable::write_request(std::uint16_t address, uint8_t value, void *instance) noexcept
{
    auto *self = static_cast<PatternTable *>(instance);

    return Cartridge::write_request(address, value, &self->cartridge_);
}
