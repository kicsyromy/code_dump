#include "device.hh"

#include <spdlog/spdlog.h>

Device::Device(device_read_f &&r, device_write_f &&w) noexcept
  : device_read_{ r }
  , device_write_{ w }
{}

void Device::connect(data_bus_t bus, bus_read_f &&r, bus_write_f &&w) noexcept
{
    data_bus_ = bus;
    bus_read_ = r;
    bus_write_ = w;
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
