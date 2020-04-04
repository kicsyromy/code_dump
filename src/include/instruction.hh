#pragma once

#include <cstdint>

struct State
{
    constexpr State() noexcept
      : modified{}
      , cycles{ 0 }
      , program_counter{ 0 }
    {}

    union
    {
        std::uint8_t fetched;
        std::uint16_t address_absolute;
        std::uint16_t address_relative;
    } modified;
    std::uint8_t cycles;
    std::uint16_t program_counter;
};

template<typename Cpu> struct Instruction
{
    const char *name;
    std::uint8_t (*opcode)(Cpu &cpu);
    State (*address_mode)(const Cpu &cpu);
    std::uint8_t cycles;
};
