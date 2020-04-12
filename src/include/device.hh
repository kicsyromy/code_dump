#pragma once

#include <spdlog/spdlog.h>

#include <cstdint>
#include <utility>

struct Device
{
private:
    using data_bus_t = void *;
    using const_data_bus_t = const void *;
    using bus_read_f = std::uint8_t (*)(std::uint16_t, bool, const_data_bus_t) noexcept;
    using bus_write_f = void (*)(std::uint16_t, std::uint8_t, data_bus_t) noexcept;

    using device_t = void *;
    using const_device_t = const void *;
    using device_read_f = std::pair<bool, std::uint8_t> (*)(std::uint16_t, const_device_t) noexcept;
    using device_write_f = bool (*)(std::uint16_t, std::uint8_t, device_t) noexcept;

public:
    Device(device_read_f && = nullptr,
        device_write_f && = nullptr,
        std::string_view name = "") noexcept;

public:
    void connect(data_bus_t bus, bus_read_f &&, bus_write_f &&) noexcept;
    std::pair<bool, std::uint8_t> read_from(std::uint16_t address) const noexcept;
    bool write_to(std::uint16_t address, std::uint8_t data) noexcept;

protected:
    inline std::uint8_t bus_read(std::uint16_t address, bool read_only = true) const noexcept
    {
        if (data_bus_ != nullptr && bus_read_ != nullptr)
            return bus_read_(address, read_only, data_bus_);

        spdlog::error("Device is not connected to a bus, read failed at address {}", address);
        return 0;
    }
    inline void bus_write(std::uint16_t address, std::uint8_t value) noexcept
    {
        if (data_bus_ != nullptr && bus_write_ != nullptr)
            bus_write_(address, value, data_bus_);
        else
            spdlog::error("Device is not connected to a bus, write failed to address {}", address);
    }

private:
    data_bus_t data_bus_{ nullptr };
    bus_read_f bus_read_{ nullptr };
    bus_write_f bus_write_{ nullptr };

    device_read_f device_read_{ nullptr };
    device_write_f device_write_{ nullptr };
    std::string_view name_{};
};
