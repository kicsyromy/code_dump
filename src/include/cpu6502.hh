#pragma once

#include "instruction.hh"
#include "register.hh"

#include <array>
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
    Cpu6502(DataBus &bus) noexcept;
    ~Cpu6502() noexcept;

public:
    Register8Bit accumulator{ 0 };
    Register8Bit x{ 0 };
    Register8Bit y{ 0 };

    Register8Bit status{ 0 };

    Register8Bit stack_pointer{ 0 };
    Register16Bit program_counter{ 0 };

private:
    void clock() noexcept;
    void reset() noexcept;
    void irq() noexcept;
    void nmi() noexcept;

private:
    void write(std::uint16_t address, std::uint8_t data) noexcept;
    std::uint8_t read(std::uint16_t address) const noexcept;
    std::uint8_t fetch(std::uint8_t opcode, const State &state) const noexcept;

private:
    DataBus &data_bus_;
    std::uint8_t remaining_cycles{ 0 };

    const std::array<Instruction<Cpu6502>, 256> instuction_set_;

private:
    friend constexpr State IMP(const Cpu6502 &) noexcept;
    friend constexpr State IMM(const Cpu6502 &) noexcept;
    friend State ZP0(const Cpu6502 &) noexcept;
    friend State ZPX(const Cpu6502 &) noexcept;
    friend State ZPY(const Cpu6502 &) noexcept;
    friend constexpr State REL(const Cpu6502 &) noexcept;
    friend State ABS(const Cpu6502 &) noexcept;
    friend State ABX(const Cpu6502 &) noexcept;
    friend State ABY(const Cpu6502 &) noexcept;
    friend State IND(const Cpu6502 &) noexcept;
    friend State IZX(const Cpu6502 &) noexcept;
    friend State IZY(const Cpu6502 &) noexcept;

private:
    friend State &ADC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &AND(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &ASL(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BCC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BCS(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BEQ(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BIT(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BMI(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BNE(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BPL(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BRK(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BVC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &BVS(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CLC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CLD(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CLI(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CLV(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CMP(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CPX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &CPY(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &DEC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &DEX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &DEY(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &EOR(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &INC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &INX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &INY(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &JMP(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &JSR(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &LDA(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &LDX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &LDY(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &LSR(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &NOP(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &ORA(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &PHA(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &PHP(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &PLA(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &PLP(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &ROL(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &ROR(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &RTI(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &RTS(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &SBC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &SEC(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &SED(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &SEI(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &STA(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &STX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &STY(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &TAX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &TAY(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &TSX(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &TXA(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &TXS(Cpu6502 &, std::uint8_t, State &) noexcept;
    friend State &TYA(Cpu6502 &, std::uint8_t, State &) noexcept;
};
