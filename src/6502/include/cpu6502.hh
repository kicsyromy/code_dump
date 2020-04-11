#pragma once

#include "device.hh"
#include "instruction.hh"
#include "register.hh"

#include <array>
#include <cstdint>

template<typename Number> constexpr std::uint8_t u8(Number value) noexcept
{
    static_assert(std::is_integral_v<Number>);
    return static_cast<std::uint8_t>(value);
}

template<typename Number> constexpr std::uint16_t u16(Number value) noexcept
{
    static_assert(std::is_integral_v<Number>);
    return static_cast<std::uint16_t>(value);
}

class Cpu6502 : public Device
{
public:
    static constexpr std::uint16_t STACK_POINTER_OFFSET{ 0x0100 };
    static constexpr std::uint16_t RESET_ADDRESS_LOCATION{ 0xFFFC };
    static constexpr std::uint16_t USER_IRQ_VECTOR_ADDRESS{ 0xFFFE };
    static constexpr std::uint16_t NMI_IRQ_VECTOR_ADDRESS{ 0xFFFA };

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
    Cpu6502() noexcept;
    ~Cpu6502() noexcept;

public:
    Register8Bit accumulator{ 0 };
    Register8Bit x{ 0 };
    Register8Bit y{ 0 };

    Register8Bit status{ 0 };

    Register8Bit stack_pointer{ 0 };
    Register16Bit program_counter{ 0 };

public:
    void draw_ram_content(std::uint16_t offset, int rows, int columns) const noexcept;
    void draw_cpu_state(int window_with, int window_height) const noexcept;

public:
    void reset() noexcept;
    void clock() noexcept;
    void irq() noexcept;
    void nmi() noexcept;
    inline bool cycle_complete() const noexcept { return remaining_cycles == 0; }

public:
    inline Instruction<Cpu6502> instruction_for_opcode(std::uint8_t opcode) const noexcept
    {
        return instuction_set_[opcode];
    }

private:
    inline uint8_t read(uint16_t address) const noexcept
    {
        return Device::bus_read(address, false);
    }
    void write(std::uint16_t address, std::uint8_t data) noexcept
    {
        Device::bus_write(address, data);
    }

private:
    const std::array<Instruction<Cpu6502>, 256> instuction_set_;
    std::uint8_t remaining_cycles{ 0 };

    /* Addressing modes */
private:
    friend constexpr State IMP(const Cpu6502 &) noexcept;
    friend constexpr State IMM(const Cpu6502 &) noexcept;
    friend State ZP0(const Cpu6502 &) noexcept;
    friend State ZPX(const Cpu6502 &) noexcept;
    friend State ZPY(const Cpu6502 &) noexcept;
    friend State REL(const Cpu6502 &) noexcept;
    friend State ABS(const Cpu6502 &) noexcept;
    friend State ABX(const Cpu6502 &) noexcept;
    friend State ABY(const Cpu6502 &) noexcept;
    friend State IND(const Cpu6502 &) noexcept;
    friend State IZX(const Cpu6502 &) noexcept;
    friend State IZY(const Cpu6502 &) noexcept;

    /* Instructions */
private:
    friend std::uint8_t fetch_argument_data(Cpu6502 &cpu, std::uint8_t, const State &) noexcept;

    friend std::uint8_t RTI(Cpu6502 &) noexcept;
    friend std::uint8_t RTS(Cpu6502 &) noexcept;

    friend std::uint8_t BRK(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t JSR(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t PHA(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t PHP(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t PLA(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t PLP(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t STA(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t STX(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t STY(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t TAX(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t TAY(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t TSX(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t TXA(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t TXS(Cpu6502 &, const State &) noexcept;
    friend std::uint8_t TYA(Cpu6502 &, const State &) noexcept;

    friend std::uint8_t ADC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t AND(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t ASL(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BCC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BCS(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BEQ(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BIT(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BMI(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BNE(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BPL(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BVC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t BVS(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CLC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CLD(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CLI(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CLV(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CMP(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CPX(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t CPY(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t DEC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t DEX(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t DEY(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t EOR(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t INC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t INX(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t INY(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t JMP(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t LDA(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t LDX(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t LDY(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t LSR(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t NOP(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t ORA(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t ROL(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t ROR(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t SBC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t SEC(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t SED(Cpu6502 &, std::uint8_t, const State &) noexcept;
    friend std::uint8_t SEI(Cpu6502 &, std::uint8_t, const State &) noexcept;
};
