#include "device.hh"

#include <spdlog/spdlog.h>

Device::Device(device_read_f &&r, device_write_f &&w, std::string_view name) noexcept
  : device_read_{ r }
  , device_write_{ w }
  , name_{ name }
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
    spdlog::error(
        "Reading from device {} with missing read function, address: 0x{:04}", name_, address);
    return { false, std::uint8_t(0) };
}

bool Device::write_to(std::uint16_t address, std::uint8_t data) noexcept
{
    if (device_write_ != nullptr) { return device_write_(address, data, this); }
    spdlog::error("Writting {:03} to read-only device {}, address: 0x{:04X}", data, name_, address);
    return false;
}
