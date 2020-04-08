#pragma once

#include "device.hh"

#include <array>

class NameTable : public Device
{
public:
    NameTable() noexcept;

private:
    static std::pair<bool, std::uint8_t> read_data(std::uint16_t address, const void *) noexcept;
    static bool write_data(std::uint16_t address, std::uint8_t value, void *) noexcept;

private:
    std::array<std::array<std::uint8_t, 1024>, 2> memory_;
};
