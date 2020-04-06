#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

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
    using instruction_f = std::uint8_t (*)(Cpu &, std::uint8_t, const State &) noexcept;
    using instruction_no_opcode = std::uint8_t (*)(Cpu &, const State &) noexcept;
    using instruction_no_params = std::uint8_t (*)(Cpu &) noexcept;

    const std::string_view name;
    std::variant<instruction_f, instruction_no_opcode, instruction_no_params> operation;
    State (*address_mode)(const Cpu &cpu) noexcept;
    std::uint8_t cycles;
};
