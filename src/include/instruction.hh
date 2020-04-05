#pragma once

#include <cstdint>
#include <string_view>

struct State
{
    constexpr State() noexcept
      : data{}
      , additional_cycles_used{ 0 }
      , processed{ 0 }
    {}

    union
    {
        std::uint8_t fetched;
        std::uint16_t address_absolute;
        std::uint16_t address_relative;
    } data;
    std::uint8_t additional_cycles_used;
    std::uint16_t processed;
};

template<typename Cpu> struct Instruction
{
    const std::string_view name;
    State &(*operation)(Cpu &cpu, std::uint8_t opcode, State &state) noexcept;
    State (*address_mode)(const Cpu &cpu) noexcept;
    std::uint8_t cycles;
};
