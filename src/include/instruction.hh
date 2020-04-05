#pragma once

#include <cstdint>
#include <string_view>

struct State
{
    constexpr State() noexcept
      : data{}
      , cycles{ 0 }
      , program_counter{ 0 }
    {}

    union
    {
        std::uint8_t fetched;
        std::uint16_t address_absolute;
        std::uint16_t address_relative;
    } data;
    std::uint8_t cycles;
    std::uint16_t program_counter;
};

template<typename Cpu> struct Instruction
{
    const std::string_view name;
    State &(*operation)(Cpu &cpu, std::uint8_t opcode, State &state) noexcept;
    State (*address_mode)(const Cpu &cpu) noexcept;
    std::uint8_t cycles;
};
