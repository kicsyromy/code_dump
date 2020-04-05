/* http://www.emulator101.com/6502-addressing-modes.html */

#pragma once

#include "cpu6502.hh"
#include "instruction.hh"

#define address_mode_private_stringify(x) #x
#define address_mode_to_string(am) address_mode_private_stringify(am)

#define address_mode_implied IMP
#define address_mode_immediate IMM
#define address_mode_zero_page ZP0
#define address_mode_zero_page_x_offset ZPX
#define address_mode_zero_page_y_offset ZPY
#define address_mode_relative REL
#define address_mode_absolute ABS
#define address_mode_absolute_x_offset ABX
#define address_mode_absolute_y_offset ABY
#define address_mode_indirect IND
#define address_mode_indirect_x IZX
#define address_mode_indirect_y IZY

// Address Mode: Implied
// There is no additional data required for this instruction. The instruction
// does something very simple like like sets a status bit. However, we will
// target the accumulator, for instructions like PHA
constexpr inline State address_mode_implied(const Cpu6502 &cpu) noexcept
{
    State result;
    result.data.fetched = cpu.accumulator.get();
    return result;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
constexpr inline State address_mode_immediate(const Cpu6502 &cpu) noexcept
{
    const auto pc = cpu.program_counter.get();

    State result;
    result.data.address_absolute = pc;
    result.processed = 1;
    return result;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
inline State address_mode_zero_page(const Cpu6502 &cpu) noexcept
{
    const auto pc = cpu.program_counter.get();

    State result;
    result.data.address_absolute = cpu.read(pc);
    result.processed = 1;
    result.data.address_absolute &= 0x00FF;
    return result;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
inline State address_mode_zero_page_x_offset(const Cpu6502 &cpu) noexcept
{
    State result = address_mode_zero_page(cpu);
    result.data.address_absolute = u16(result.data.address_absolute + cpu.x.get());
    result.data.address_absolute &= 0x00FF;
    return result;
}

// Address Mode: Zero Page with Y Offset
// Same as above but uses Y Register for offset
inline State address_mode_zero_page_y_offset(const Cpu6502 &cpu) noexcept
{
    State result = address_mode_zero_page(cpu);
    result.data.address_absolute = u16(result.data.address_absolute + cpu.y.get());
    result.data.address_absolute &= 0x00FF;
    return result;
}

// Address Mode: Relative
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
constexpr inline State address_mode_relative(const Cpu6502 &cpu) noexcept
{
    const auto pc = cpu.program_counter.get();

    State result;
    result.data.address_relative = pc;
    result.processed = 1;
    if (result.data.address_relative & 0x80) result.data.address_relative |= 0xFF00;
    return result;
}

// Address Mode: Absolute
// A full 16-bit address is loaded and used
inline State address_mode_absolute(const Cpu6502 &cpu) noexcept
{
    const auto pc = cpu.program_counter.get();

    const auto low_byte = u16(cpu.read(pc));
    const auto high_byte = u16(cpu.read(u16(pc + 1)));

    State result;
    result.data.address_absolute = u16((high_byte << 8) | low_byte);
    result.processed = 2;
    return result;
}

// Address Mode: Absolute with X Offset
// Fundamentally the same as absolute addressing, but the contents of the X Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
inline State address_mode_absolute_x_offset(const Cpu6502 &cpu) noexcept
{
    State result = address_mode_absolute(cpu);
    const auto high_byte = u16(result.data.address_absolute >> 8);
    result.data.address_absolute = u16(result.data.address_absolute + cpu.x.get());
    if ((result.data.address_absolute & 0xFF00) != (high_byte << 8))
        result.additional_cycles_used = 1;
    else
        result.additional_cycles_used = 0;
    return result;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
inline State address_mode_absolute_y_offset(const Cpu6502 &cpu) noexcept
{
    State result = address_mode_absolute(cpu);
    const auto high_byte = u16(result.data.address_absolute >> 8);
    result.data.address_absolute = u16(result.data.address_absolute + cpu.y.get());
    if ((result.data.address_absolute & 0xFF00) != (high_byte << 8))
        result.additional_cycles_used = 1;
    else
        result.additional_cycles_used = 0;
    return result;
}

// Address Mode: Indirect
// The supplied 16-bit address is read to get the actual 16-bit address. This is
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as
// designed, instead it wraps back around in the same page, yielding an
// invalid actual address
inline State address_mode_indirect(const Cpu6502 &cpu) noexcept
{
    const auto pc = cpu.program_counter.get();

    const auto low_byte = u16(cpu.read(pc));
    const auto high_byte = u16(cpu.read(u16(pc + 1)));

    const auto pointer = u16((high_byte << 8) | low_byte);

    State result;
    if (low_byte == 0x00FF) /* Simulate page boundary hardware bug */
    {
        result.data.address_absolute =
            u16((cpu.read(pointer & 0xFF00) << 8) | cpu.read(pointer + 0));
    }
    else
    {
        result.data.address_absolute =
            u16((cpu.read(u16(pointer + 1)) << 8) | cpu.read(pointer + 0));
    }
    result.processed = 2;
    return result;
}

// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read
// from this location
inline State address_mode_indirect_x(const Cpu6502 &cpu) noexcept
{
    auto pc = cpu.program_counter.get();

    const auto t = u16(cpu.read(pc));
    const auto low_byte = cpu.read(u16((t + u16(cpu.x.get())) & 0x00FF));
    const auto high_byte = cpu.read(u16((t + static_cast<uint16_t>(cpu.x.get() + 1)) & 0x00FF));

    State result;
    result.data.address_absolute = u16((high_byte << 8) | low_byte);
    result.processed = 1;
    return result;
}

// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
inline State address_mode_indirect_y(const Cpu6502 &cpu) noexcept
{
    auto pc = cpu.program_counter.get();

    const auto t = u16(cpu.read(pc));
    const auto low_byte = cpu.read(u16(t & 0x00FF));
    const auto high_byte = cpu.read(u16(u16(t + 1) & 0x00FF));

    State result;
    result.data.address_absolute = u16(((high_byte << 8) | low_byte) + cpu.y.get());
    result.processed = 1;
    if ((result.data.address_absolute & 0xFF00) != (high_byte << 8))
        result.additional_cycles_used = 1;
    else
        result.additional_cycles_used = 0;
    return result;
}
