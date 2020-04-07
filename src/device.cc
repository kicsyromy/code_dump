#include "device.hh"

#include <spdlog/spdlog.h>

Device::Device(std::size_t address_start,
    std::size_t address_end,
    device_read_f &&r,
    device_write_f &&w) noexcept
  : device_read_{ r }
  , device_write_{ w }
  , address_start_{ address_start }
  , address_end_{ address_end }
{}

void Device::connect(data_bus_t bus, bus_read_f &&r, bus_write_f &&w) noexcept
{
    data_bus_ = bus;
    bus_read_ = r;
    bus_write_ = w;
}

bool Device::handles_address(std::uint16_t address) const noexcept
{
    return address >= address_start_ && address <= address_end_;
}

std::pair<bool, std::uint8_t> Device::read_from(std::uint16_t address) const noexcept
{
    if (device_read_ != nullptr) { return device_read_(address, this); }
    spdlog::error("Reading from device with missing read function");
    return { false, 0 };
}

bool Device::write_to(std::uint16_t address, std::uint8_t data) noexcept
{
    if (device_write_ != nullptr) { return device_write_(address, data, this); }
    spdlog::error("Writting to read-only device");
    return false;
}
