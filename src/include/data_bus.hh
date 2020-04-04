#pragma once

#include "cpu6502.hh"

#include <array>
#include <cstdint>

class DataBus
{
public:
    DataBus() noexcept;
    ~DataBus() noexcept;

public:
    Cpu6502 CPU;
    std::array<std::uint8_t, 64 * 1024> RAM{};

public:
    void write(std::uint16_t address, std::uint8_t data) noexcept;
    [[nodiscard]] std::uint8_t read(std::uint16_t address, bool read_only = false) noexcept;

private:
};
