#pragma once

#include "device.hh"

#include <array>

class Ram : public Device
{
public:
    Ram() noexcept;

private:
    static std::pair<bool, std::uint8_t> read_data(std::uint16_t address, const void *) noexcept;
    static bool write_data(std::uint16_t address, std::uint8_t value, void *) noexcept;

public:
    std::array<std::uint8_t, 64 * 1024> RAM{};
};
