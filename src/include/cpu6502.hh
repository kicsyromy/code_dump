#pragma once

#include "register.hh"

#include <cstdint>

class DataBus;

class Cpu6502
{
public:
    enum StatusFlags : std::uint8_t
    {
        /* clang-format off */
        Carry             = (1 << 0),
        Zero              = (1 << 1),
        DisableInterrupts = (1 << 2), /* Unused in this implementation */
        DecimalMode       = (1 << 3),
        Break             = (1 << 4),
        Unused            = (1 << 5),
        Overflow          = (1 << 6),
        Negative          = (1 << 7)
        /* clang-format on */
    };

public:
    Register8Bit accumulator{ 0 };
    Register8Bit x{ 0 };
    Register8Bit y{ 0 };

    Register8Bit status{ 0 };

    Register8Bit stack_pointer{ 0 };
    Register16Bit program_counter{ 0 };

public:
    Cpu6502(DataBus &bus) noexcept;
    ~Cpu6502() noexcept;

public:
    void write(std::uint16_t address, std::uint8_t data) noexcept;
    std::uint8_t read(std::uint16_t address) const noexcept;

private:
    DataBus &data_bus_;
};
