#pragma once

#include "cpu6502.hh"

#include <array>
#include <cstdint>
#include <tuple>

template<typename... Devices> class DataBus
{
private:
    static std::uint8_t read(std::uint16_t address, bool read_only, void *instance) noexcept
    {
        auto self = static_cast<DataBus *>(instance);
        return self->read(address, read_only);
    }
    static void write(std::uint16_t address, std::uint8_t data, void *instance) noexcept
    {
        auto self = static_cast<DataBus *>(instance);
        self->write(address, data);
    };

public:
    DataBus(Devices &... devices) noexcept
      : connected_devices_{ std::addressof(devices)... }
    {
        std::apply([this](auto &... device) { (device->connect(this, &read, &write), ...); },
            connected_devices_);
    }

    ~DataBus() noexcept = default;

public:
    std::tuple<Devices *...> connected_devices_{};
    std::array<std::uint8_t, 64 * 1024> RAM{};

public:
    [[nodiscard]] std::uint8_t read(std::uint16_t address, [[maybe_unused]] bool read_only) noexcept
    {

        // if (address >= 0x0000 && address <= 0xFFFE) {
        return RAM[address];
        // }
        // return 0x00;
    }

    void write(std::uint16_t address, std::uint8_t data) noexcept
    {
        // if (address >= 0x0000 && address <= 0xFFFF) {
        RAM[address] = data;
        // }
    }
};
