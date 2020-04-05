#pragma once

#include "cpu6502.hh"
#include "address_modes.hh"

#include <cstdint>

#define opcode_private_stringify(x) #x
#define opcode_to_string(am) opcode_private_stringify(am)

#define opcode_illegal_unknown UNK
#define opcode_add_with_carry_in ADC
#define opcode_bitwise_and AND
#define opcode_arithmetic_shift_left ASL
#define opcode_branch_if_carry_clear BCC
#define opcode_branch_if_carry_set BCS
#define opcode_branch_if_equal BEQ
#define opcode_test_bits BIT
#define opcode_branch_if_negative BMI
#define opcode_branch_if_not_equal BNE
#define opcode_branch_if_positive BPL
#define opcode_break BRK
#define opcode_branch_if_overflow_clear BVC
#define opcode_branch_if_overflow_set BVS
#define opcode_clear_carry CLC
#define opcode_clear_decimal CLD
#define opcode_disable_interrupts CLI
#define opcode_clear_overflow CLV
#define opcode_compare_accumulator CMP
#define opcode_compare_x CPX
#define opcode_compare_y CPY
#define opcode_decrement_value_at_location DEC
#define opcode_decrement_x DEX
#define opcode_decrement_y DEY
#define opcode_bitwise_xor EOR
#define opcode_increment_value_at_location INC
#define opcode_increment_x INX
#define opcode_increment_y INY
#define opcode_jump_to_location JMP
#define opcode_jump_to_subroutine JSR
#define opcode_load_accumulator LDA
#define opcode_load_x_register LDX
#define opcode_load_y_register LDY
#define opcode_logical_shift_right LSR
#define opcode_no_operation NOP
#define opcode_bitwise_logic_or ORA
#define opcode_push_accumulator_to_stack PHA
#define opcode_push_status_register_to_stack PHP
#define opcode_pop_accumulator_from_stack PLA
#define opcode_pop_status_register_from_stack PLP
#define opcode_rotate_left ROL
#define opcode_rotate_right ROR
#define opcode_return_from_interrupt RTI
#define opcode_return_from_subroutine RTS
#define opcode_substraction_borrow_in SBC
#define opcode_set_carry_flag SEC
#define opcode_set_decimal_flag SED
#define opcode_enable_interrupts SEI
#define opcode_store_accumulator_at_address STA
#define opcode_store_x_register_at_address STX
#define opcode_store_y_register_at_address STY
#define opcode_transfer_accumulator_to_x TAX
#define opcode_transfer_accumulator_to_y TAY
#define opcode_transfer_stack_pointer_to_x TSX
#define opcode_transfer_stack_pointer_to_accumulator TXA
#define opcode_transfer_x_to_stack_pointer TXS
#define opcode_transfer_accumulator_to_stack_pointer TYA

#define a() cpu.accumulator.get()
#define x() cpu.x.get()
#define y() cpu.y.get()
#define status() cpu.status.get()
#define sp() cpu.stack_pointer.get()
#define pc() cpu.program_counter.get()

enum StatusFlags : std::uint8_t
{
    /* clang-format off */
    Carry             = Cpu6502::Carry,
    Zero              = Cpu6502::Zero,
    DisableInterrupts = Cpu6502::DisableInterrupts, /* Unused in this implementation */
    DecimalMode       = Cpu6502::DecimalMode,
    Break             = Cpu6502::Break,
    Unused            = Cpu6502::Unused,
    Overflow          = Cpu6502::Overflow,
    Negative          = Cpu6502::Negative
    /* clang-format on */
};

inline State &opcode_add_with_carry_in(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);

    // Add is performed in 16-bit domain for emulation to capture any
    // carry bit, which will exist in bit 8 of the 16-bit word
    const std::uint16_t temp =
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(a()) + fetched +
                                   static_cast<std::uint16_t>(cpu.status.is_flag_set(Carry)));

    cpu.status.set_flag(Carry, temp > 255);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Overflow,
        (~(static_cast<std::uint16_t>(a()) ^ fetched) & (static_cast<std::uint16_t>(a()) ^ temp)) &
            0x0080);
    cpu.status.set_flag(Negative, temp & 0x80);
    a() = static_cast<std::uint8_t>(temp & 0x00FF);

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_bitwise_and(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const std::uint8_t fetched = cpu.fetch(opcode, state);

    const std::uint8_t temp = u8(a() & fetched);

    cpu.status.set_flag(Zero, temp == 0);
    cpu.status.set_flag(Negative, temp & 0x80);

    a() = temp;

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_arithmetic_shift_left(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);

    const auto temp = u16(fetched << 1);
    cpu.status.set_flag(Carry, (temp & 0XFF00) > 0);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x80);

    if (cpu.instuction_set_[opcode].address_mode == &address_mode_implied)
    { a() = u8(temp & 0x00FF); }
    else
    {
        cpu.write(state.data.address_absolute, u8(temp & 0x00FF));
    }

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_branch([[maybe_unused]] Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state,
    bool condition) noexcept
{
    if (condition)
    {
        const auto pc = pc() + state.processed;

        state.additional_cycles_used = 1;
        const auto absolute_address = pc + state.data.address_relative;

        if ((absolute_address & 0xFF00) != (pc & absolute_address))
        { state.additional_cycles_used = 2; }

        state.processed = u16(state.processed + state.data.address_relative);
    }

    return state;
}

inline State &opcode_branch_if_carry_clear(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, !cpu.status.is_flag_set(Carry));
}

inline State &opcode_branch_if_carry_set(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, cpu.status.is_flag_set(Carry));
}

inline State &opcode_branch_if_equal(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, cpu.status.is_flag_set(Zero));
}

inline State &opcode_branch_if_negative(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, cpu.status.is_flag_set(Negative));
}

inline State &opcode_branch_if_not_equal(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, !cpu.status.is_flag_set(Zero));
}

inline State &opcode_branch_if_positive(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, !cpu.status.is_flag_set(Negative));
}

inline State &opcode_branch_if_overflow_clear(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, !cpu.status.is_flag_set(Overflow));
}

inline State &opcode_branch_if_overflow_set(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    return opcode_branch(cpu, opcode, state, cpu.status.is_flag_set(Overflow));
}

inline State &opcode_break(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto pc = u16(pc() + state.processed + 1);

    cpu.status.set_flag(DisableInterrupts, true);
    cpu.write(u16(Cpu6502::STACK_POINTER_OFFSET + sp()), u8((pc >> 8) & 0x00FF));
    --(sp());
    cpu.write(u16(Cpu6502::STACK_POINTER_OFFSET + sp()), u8((pc & 0x00FF)));
    --(sp());

    cpu.status.set_flag(Break, true);
    cpu.write(u16(Cpu6502::STACK_POINTER_OFFSET + sp()), status());
    --(sp());
    cpu.status.set_flag(Break, false);

    cpu.program_counter.low_byte() = cpu.read(Cpu6502::IRQ_USER_TABLE_ADDRESS);
    cpu.program_counter.high_byte() = cpu.read(0xFFFF);

    state.processed = 0;
    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_test_bits(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = cpu.fetch(opcode, state);
    const auto temp = u8(a() & fetched);

    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0x00);
    cpu.status.set_flag(Negative, fetched & 0x80);
    cpu.status.set_flag(Overflow, fetched & (1 << 6));

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_clear_carry(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(Carry, false);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_clear_decimal(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(DecimalMode, false);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_disable_interrupts(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(DisableInterrupts, true);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_clear_overflow(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(Overflow, false);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_compare_accumulator(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);
    const auto temp = u16(u16(a()) - fetched);

    cpu.status.set_flag(Carry, a() >= fetched);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_compare_x(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);
    const auto temp = u16(u16(x()) - fetched);

    cpu.status.set_flag(Carry, x() >= fetched);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_compare_y(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);
    const auto temp = u16(u16(y()) - fetched);

    cpu.status.set_flag(Carry, y() >= fetched);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_decrement_value_at_location(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);
    const auto temp = u16(fetched - 1);

    cpu.write(state.data.address_absolute, u8(temp & 0x00FF));

    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_decrement_x(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    --(x());
    cpu.status.set_flag(Zero, x() == 0);
    cpu.status.set_flag(Negative, x() & 0x80);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_decrement_y(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    --(y());
    cpu.status.set_flag(Zero, y() == 0);
    cpu.status.set_flag(Negative, y() & 0x80);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_bitwise_xor(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = cpu.fetch(opcode, state);
    const auto temp = u8(a() ^ fetched);

    cpu.status.set_flag(Zero, temp == 0);
    cpu.status.set_flag(Negative, temp & 0x80);

    a() = temp;

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_increment_value_at_location(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);
    const auto temp = u16(fetched + 1);

    cpu.write(state.data.address_absolute, u8(temp & 0x00FF));

    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_increment_x(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    ++(x());
    cpu.status.set_flag(Zero, x() == 0);
    cpu.status.set_flag(Negative, x() & 0x80);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_increment_y(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    ++(y());
    cpu.status.set_flag(Zero, y() == 0);
    cpu.status.set_flag(Negative, y() & 0x80);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_jump_to_location(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    pc() = state.data.address_absolute;

    state.processed = 0;
    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_jump_to_subroutine(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto pc = u16(pc() + state.processed);
    cpu.write(u16(Cpu6502::STACK_POINTER_OFFSET + sp()), u8((pc >> 8) & 0x00FF));
    --(sp());
    cpu.write(u16(Cpu6502::STACK_POINTER_OFFSET + sp()), u8(pc & 0x00FF));
    --(sp());

    pc() = state.data.address_absolute;

    state.processed = 0;
    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_load_accumulator(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = cpu.fetch(opcode, state);

    cpu.status.set_flag(Zero, fetched == 0);
    cpu.status.set_flag(Negative, fetched & 0x80);

    a() = fetched;

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_load_x_register(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = cpu.fetch(opcode, state);

    cpu.status.set_flag(Zero, fetched == 0);
    cpu.status.set_flag(Negative, fetched & 0x80);

    x() = fetched;

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_load_y_register(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = cpu.fetch(opcode, state);

    cpu.status.set_flag(Zero, fetched == 0);
    cpu.status.set_flag(Negative, fetched & 0x80);

    y() = fetched;

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_logical_shift_right(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = u16(cpu.fetch(opcode, state));

    cpu.status.set_flag(Carry, fetched & 0x0001);

    const auto temp = u16(fetched >> 1);

    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    if (cpu.instuction_set_[opcode].address_mode == &address_mode_implied)
    { a() = u8(temp & 0x00FF); }
    else
    {
        cpu.write(state.data.address_absolute, u8(temp & 0x00FF));
    }

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_no_operation([[maybe_unused]] Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    switch (opcode)
    {
    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
        state.additional_cycles_used &= 1;
        break;
    default:
        state.additional_cycles_used = 0;
        break;
    }

    return state;
}

inline State &opcode_bitwise_logic_or(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = cpu.fetch(opcode, state);
    const auto temp = u8(a() | fetched);

    cpu.status.set_flag(Zero, temp == 0);
    cpu.status.set_flag(Negative, temp & 0x80);

    a() = temp;

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_push_accumulator_to_stack(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.write(u16(Cpu6502::STACK_POINTER_OFFSET + sp()), a());
    --(sp());

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_push_status_register_to_stack(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    cpu.write(Cpu6502::STACK_POINTER_OFFSET + sp(), u8(status() | Break | Unused));
    cpu.status.set_flag(Break, false);
    cpu.status.set_flag(Unused, false);
    --(sp());

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_pop_accumulator_from_stack(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    const auto sp = ++(sp());

    a() = cpu.read(u16(Cpu6502::STACK_POINTER_OFFSET + sp));
    cpu.status.set_flag(Zero, a() == 0);
    cpu.status.set_flag(Negative, a() & 0x80);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_pop_status_register_from_stack(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    const auto sp = ++(sp());
    status() = cpu.read(Cpu6502::STACK_POINTER_OFFSET + sp);
    cpu.status.set_flag(Unused, true);

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_rotate_left(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = u16(cpu.fetch(opcode, state));
    const auto temp = u16((fetched << 1) | cpu.status.is_flag_set(Carry));

    cpu.status.set_flag(Carry, temp & 0xFF00);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    if (cpu.instuction_set_[opcode].address_mode == &address_mode_implied)
    { a() = u8(temp & 0x00FF); }
    else
    {
        cpu.write(state.data.address_absolute, u8(temp & 0x00FF));
    }

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_rotate_right(Cpu6502 &cpu, std::uint8_t opcode, State &state) noexcept
{
    const auto fetched = u16(cpu.fetch(opcode, state));
    const auto temp = u16((fetched << 7) | (fetched >> 1));

    cpu.status.set_flag(Carry, fetched & 0x01);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(Negative, temp & 0x0080);

    if (cpu.instuction_set_[opcode].address_mode == &address_mode_implied)
    { a() = u8(temp & 0x00FF); }
    else
    {
        cpu.write(state.data.address_absolute, u8(temp & 0x00FF));
    }

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_return_from_interrupt(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    auto sp = u8(sp() + 1);
    status() = cpu.read(u16(Cpu6502::STACK_POINTER_OFFSET + sp));
    status() = u8(status() & ~Break);
    status() = u8(status() & ~Unused);

    ++sp;
    const auto new_program_counter = u16(cpu.read(u16(Cpu6502::STACK_POINTER_OFFSET + sp)));
    ++sp;
    pc() = u16(new_program_counter | u16(cpu.read(u16(Cpu6502::STACK_POINTER_OFFSET + sp)) << 8));
    sp() = sp;

    state.processed = 0;
    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_return_from_subroutine(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    auto sp = u8(sp() + 1);
    auto new_program_counter = u16(cpu.read(u16(Cpu6502::STACK_POINTER_OFFSET + sp)));
    ++sp;
    new_program_counter =
        u16(new_program_counter | u16(cpu.read(u16(Cpu6502::STACK_POINTER_OFFSET + sp))));

    pc() = u16(new_program_counter + 1);
    sp() = sp;

    state.additional_cycles_used = 0;
    return state;
}

inline State &opcode_substraction_borrow_in(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    const std::uint16_t fetched = cpu.fetch(opcode, state);
    const std::uint16_t value = (static_cast<std::uint16_t>(fetched)) ^ 0x00FF;

    const std::uint16_t temp =
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(a()) + value +
                                   static_cast<std::uint16_t>(cpu.status.is_flag_set(Carry)));
    cpu.status.set_flag(Carry, temp > 255);
    cpu.status.set_flag(Zero, (temp & 0x00FF) == 0);
    cpu.status.set_flag(
        Overflow, ((temp ^ static_cast<std::uint16_t>(a())) & (fetched ^ value)) & 0x0080);
    cpu.status.set_flag(Negative, temp & 0x80);
    a() = static_cast<std::uint8_t>(temp & 0x00FF);

    state.additional_cycles_used &= 1;
    return state;
}

inline State &opcode_set_carry_flag(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(Carry, true);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_set_decimal_flag(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(DecimalMode, true);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_enable_interrupts(Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    cpu.status.set_flag(DisableInterrupts, false);
    state.additional_cycles_used = 0;

    return state;
}

inline State &opcode_store_accumulator_at_address(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_store_x_register_at_address(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_store_y_register_at_address(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_transfer_accumulator_to_x(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_transfer_accumulator_to_y(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_transfer_stack_pointer_to_x(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_transfer_stack_pointer_to_accumulator(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_transfer_x_to_stack_pointer(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_transfer_accumulator_to_stack_pointer(Cpu6502 &cpu,
    std::uint8_t opcode,
    State &state) noexcept
{
    static_cast<void>(cpu);
    static_cast<void>(opcode);
    static_cast<void>(state);
    return state;
}

inline State &opcode_illegal_unknown([[maybe_unused]] Cpu6502 &cpu,
    [[maybe_unused]] std::uint8_t opcode,
    State &state) noexcept
{
    return state;
}
