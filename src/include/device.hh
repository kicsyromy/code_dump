#pragma once

#include <cstdint>
#include <utility>

struct Device
{
public:
    using data_bus_t = void *;
    using bus_read_f = std::uint8_t (*)(std::uint16_t, bool, const void *) noexcept;
    using bus_write_f = void (*)(std::uint16_t, std::uint8_t, data_bus_t) noexcept;

    using device_read_f = std::pair<bool, std::uint8_t> (*)(std::uint16_t, const void *) noexcept;
    using device_write_f = bool (*)(std::uint16_t, std::uint8_t, void *) noexcept;

public:
    Device(device_read_f && = nullptr, device_write_f && = nullptr) noexcept;

public:
    void connect(data_bus_t bus, bus_read_f &&, bus_write_f &&) noexcept;
    std::pair<bool, std::uint8_t> read_from(std::uint16_t address) const noexcept;
    bool write_to(std::uint16_t address, std::uint8_t data) noexcept;

protected:
    data_bus_t data_bus_{ nullptr };
    bus_read_f bus_read_{ nullptr };
    bus_write_f bus_write_{ nullptr };

    device_read_f device_read_{ nullptr };
    device_write_f device_write_{ nullptr };
};
