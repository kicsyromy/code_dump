#include "cpu6502.hh"

#include "address_modes.hh"
#include "data_bus.hh"
#include "instruction_set.hh"

/* clang-format off */
#define instruction(ins, addr_mode, cycle_cnt) { #ins, &ins, &addr_mode, cycle_cnt }
/* clang-format on */

Cpu6502::Cpu6502(DataBus &bus) noexcept
  : data_bus_{ bus }
  , instuction_set_{ {
        /* replace all of this with instruction macro */
        /* clang-format off */
        { "BRK", &BRK, &IMM, 7 }, { "ORA", &ORA, &IZX, 6 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 3 }, { "ORA", &ORA, &ZP0, 3 }, { "ASL", &ASL, &ZP0, 5 }, { "???", &UNK, &IMP, 5 },
        { "PHP", &PHP, &IMP, 3 }, { "ORA", &ORA, &IMM, 2 }, { "ASL", &ASL, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "???", &NOP, &IMP, 4 }, { "ORA", &ORA, &ABS, 4 }, { "ASL", &ASL, &ABS, 6 }, { "???", &UNK, &IMP, 6 },
        { "BPL", &BPL, &REL, 2 }, { "ORA", &ORA, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 4 }, { "ORA", &ORA, &ZPX, 4 }, { "ASL", &ASL, &ZPX, 6 }, { "???", &UNK, &IMP, 6 },
        { "CLC", &CLC, &IMP, 2 }, { "ORA", &ORA, &ABY, 4 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 4 }, { "ORA", &ORA, &ABX, 4 }, { "ASL", &ASL, &ABX, 7 }, { "???", &UNK, &IMP, 7 },
        { "JSR", &JSR, &ABS, 6 }, { "AND", &AND, &IZX, 6 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "BIT", &BIT, &ZP0, 3 }, { "AND", &AND, &ZP0, 3 }, { "ROL", &ROL, &ZP0, 5 }, { "???", &UNK, &IMP, 5 },
        { "PLP", &PLP, &IMP, 4 }, { "AND", &AND, &IMM, 2 }, { "ROL", &ROL, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "BIT", &BIT, &ABS, 4 }, { "AND", &AND, &ABS, 4 }, { "ROL", &ROL, &ABS, 6 }, { "???", &UNK, &IMP, 6 },
        { "BMI", &BMI, &REL, 2 }, { "AND", &AND, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 4 }, { "AND", &AND, &ZPX, 4 }, { "ROL", &ROL, &ZPX, 6 }, { "???", &UNK, &IMP, 6 },
        { "SEC", &SEC, &IMP, 2 }, { "AND", &AND, &ABY, 4 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 4 }, { "AND", &AND, &ABX, 4 }, { "ROL", &ROL, &ABX, 7 }, { "???", &UNK, &IMP, 7 },
        { "RTI", &RTI, &IMP, 6 }, { "EOR", &EOR, &IZX, 6 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 3 }, { "EOR", &EOR, &ZP0, 3 }, { "LSR", &LSR, &ZP0, 5 }, { "???", &UNK, &IMP, 5 },
        { "PHA", &PHA, &IMP, 3 }, { "EOR", &EOR, &IMM, 2 }, { "LSR", &LSR, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "JMP", &JMP, &ABS, 3 }, { "EOR", &EOR, &ABS, 4 }, { "LSR", &LSR, &ABS, 6 }, { "???", &UNK, &IMP, 6 },
        { "BVC", &BVC, &REL, 2 }, { "EOR", &EOR, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 4 }, { "EOR", &EOR, &ZPX, 4 }, { "LSR", &LSR, &ZPX, 6 }, { "???", &UNK, &IMP, 6 },
        { "CLI", &CLI, &IMP, 2 }, { "EOR", &EOR, &ABY, 4 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 4 }, { "EOR", &EOR, &ABX, 4 }, { "LSR", &LSR, &ABX, 7 }, { "???", &UNK, &IMP, 7 },
        { "RTS", &RTS, &IMP, 6 }, { "ADC", &ADC, &IZX, 6 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 3 }, { "ADC", &ADC, &ZP0, 3 }, { "ROR", &ROR, &ZP0, 5 }, { "???", &UNK, &IMP, 5 },
        { "PLA", &PLA, &IMP, 4 }, { "ADC", &ADC, &IMM, 2 }, { "ROR", &ROR, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "JMP", &JMP, &IND, 5 }, { "ADC", &ADC, &ABS, 4 }, { "ROR", &ROR, &ABS, 6 }, { "???", &UNK, &IMP, 6 },
        { "BVS", &BVS, &REL, 2 }, { "ADC", &ADC, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 4 }, { "ADC", &ADC, &ZPX, 4 }, { "ROR", &ROR, &ZPX, 6 }, { "???", &UNK, &IMP, 6 },
        { "SEI", &SEI, &IMP, 2 }, { "ADC", &ADC, &ABY, 4 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 4 }, { "ADC", &ADC, &ABX, 4 }, { "ROR", &ROR, &ABX, 7 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 2 }, { "STA", &STA, &IZX, 6 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 6 },
        { "STY", &STY, &ZP0, 3 }, { "STA", &STA, &ZP0, 3 }, { "STX", &STX, &ZP0, 3 }, { "???", &UNK, &IMP, 3 },
        { "DEY", &DEY, &IMP, 2 }, { "???", &NOP, &IMP, 2 }, { "TXA", &TXA, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "STY", &STY, &ABS, 4 }, { "STA", &STA, &ABS, 4 }, { "STX", &STX, &ABS, 4 }, { "???", &UNK, &IMP, 4 },
        { "BCC", &BCC, &REL, 2 }, { "STA", &STA, &IZY, 6 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 6 },
        { "STY", &STY, &ZPX, 4 }, { "STA", &STA, &ZPX, 4 }, { "STX", &STX, &ZPY, 4 }, { "???", &UNK, &IMP, 4 },
        { "TYA", &TYA, &IMP, 2 }, { "STA", &STA, &ABY, 5 }, { "TXS", &TXS, &IMP, 2 }, { "???", &UNK, &IMP, 5 },
        { "???", &NOP, &IMP, 5 }, { "STA", &STA, &ABX, 5 }, { "???", &UNK, &IMP, 5 }, { "???", &UNK, &IMP, 5 },
        { "LDY", &LDY, &IMM, 2 }, { "LDA", &LDA, &IZX, 6 }, { "LDX", &LDX, &IMM, 2 }, { "???", &UNK, &IMP, 6 },
        { "LDY", &LDY, &ZP0, 3 }, { "LDA", &LDA, &ZP0, 3 }, { "LDX", &LDX, &ZP0, 3 }, { "???", &UNK, &IMP, 3 },
        { "TAY", &TAY, &IMP, 2 }, { "LDA", &LDA, &IMM, 2 }, { "TAX", &TAX, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "LDY", &LDY, &ABS, 4 }, { "LDA", &LDA, &ABS, 4 }, { "LDX", &LDX, &ABS, 4 }, { "???", &UNK, &IMP, 4 },
        { "BCS", &BCS, &REL, 2 }, { "LDA", &LDA, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 5 },
        { "LDY", &LDY, &ZPX, 4 }, { "LDA", &LDA, &ZPX, 4 }, { "LDX", &LDX, &ZPY, 4 }, { "???", &UNK, &IMP, 4 },
        { "CLV", &CLV, &IMP, 2 }, { "LDA", &LDA, &ABY, 4 }, { "TSX", &TSX, &IMP, 2 }, { "???", &UNK, &IMP, 4 },
        { "LDY", &LDY, &ABX, 4 }, { "LDA", &LDA, &ABX, 4 }, { "LDX", &LDX, &ABY, 4 }, { "???", &UNK, &IMP, 4 },
        { "CPY", &CPY, &IMM, 2 }, { "CMP", &CMP, &IZX, 6 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "CPY", &CPY, &ZP0, 3 }, { "CMP", &CMP, &ZP0, 3 }, { "DEC", &DEC, &ZP0, 5 }, { "???", &UNK, &IMP, 5 },
        { "INY", &INY, &IMP, 2 }, { "CMP", &CMP, &IMM, 2 }, { "DEX", &DEX, &IMP, 2 }, { "???", &UNK, &IMP, 2 },
        { "CPY", &CPY, &ABS, 4 }, { "CMP", &CMP, &ABS, 4 }, { "DEC", &DEC, &ABS, 6 }, { "???", &UNK, &IMP, 6 },
        { "BNE", &BNE, &REL, 2 }, { "CMP", &CMP, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 4 }, { "CMP", &CMP, &ZPX, 4 }, { "DEC", &DEC, &ZPX, 6 }, { "???", &UNK, &IMP, 6 },
        { "CLD", &CLD, &IMP, 2 }, { "CMP", &CMP, &ABY, 4 }, { "NOP", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 4 }, { "CMP", &CMP, &ABX, 4 }, { "DEC", &DEC, &ABX, 7 }, { "???", &UNK, &IMP, 7 },
        { "CPX", &CPX, &IMM, 2 }, { "SBC", &SBC, &IZX, 6 }, { "???", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "CPX", &CPX, &ZP0, 3 }, { "SBC", &SBC, &ZP0, 3 }, { "INC", &INC, &ZP0, 5 }, { "???", &UNK, &IMP, 5 },
        { "INX", &INX, &IMP, 2 }, { "SBC", &SBC, &IMM, 2 }, { "NOP", &NOP, &IMP, 2 }, { "???", &SBC, &IMP, 2 },
        { "CPX", &CPX, &ABS, 4 }, { "SBC", &SBC, &ABS, 4 }, { "INC", &INC, &ABS, 6 }, { "???", &UNK, &IMP, 6 },
        { "BEQ", &BEQ, &REL, 2 }, { "SBC", &SBC, &IZY, 5 }, { "???", &UNK, &IMP, 2 }, { "???", &UNK, &IMP, 8 },
        { "???", &NOP, &IMP, 4 }, { "SBC", &SBC, &ZPX, 4 }, { "INC", &INC, &ZPX, 6 }, { "???", &UNK, &IMP, 6 },
        { "SED", &SED, &IMP, 2 }, { "SBC", &SBC, &ABY, 4 }, { "NOP", &NOP, &IMP, 2 }, { "???", &UNK, &IMP, 7 },
        { "???", &NOP, &IMP, 4 }, { "SBC", &SBC, &ABX, 4 }, { "INC", &INC, &ABX, 7 }, { "???", &UNK, &IMP, 7 },
        /* clang-format on */
    } }
{}

Cpu6502::~Cpu6502() noexcept = default;

void Cpu6502::clock() noexcept
{
    if (remaining_cycles == 0)
    {
        const auto opcode = read(program_counter.get());

        status.set_flag(Unused, true);

        ++(program_counter.get());

        remaining_cycles = instuction_set_[opcode].cycles;
        auto state = instuction_set_[opcode].address_mode(*this);
        instuction_set_[opcode].operation(*this, opcode, state);

        program_counter.get() = u16(program_counter.get() + state.processed);
        remaining_cycles = u8(remaining_cycles + state.additional_cycles_used);

        status.set_flag(Unused, true);
    }

    remaining_cycles--;
}

void Cpu6502::reset() noexcept
{
    accumulator.set(0);
    x.set(0);
    y.set(0);
    stack_pointer.set(0xFD);
    status.set(0x00 | Unused);

    const std::uint16_t low_byte = read(RESET_ADDRESS_LOCATION + 0);
    const std::uint16_t high_byte = read(RESET_ADDRESS_LOCATION + 1);

    program_counter.set(u16((high_byte << 8) | (low_byte & 0x00FF)));

    remaining_cycles = 8;
}

void Cpu6502::irq() noexcept
{
    if (!status.is_flag_set(DisableInterrupts))
    {
        write(u16(STACK_POINTER_OFFSET + stack_pointer.get()), program_counter.high_byte());
        --(stack_pointer.get());
        write(u16(STACK_POINTER_OFFSET + stack_pointer.get()), program_counter.low_byte());
        --(stack_pointer.get());

        status.set_flag(Break, false);
        status.set_flag(Unused, true);
        status.set_flag(DisableInterrupts, true);

        write(u16(STACK_POINTER_OFFSET + stack_pointer.get()), status.get());
        --(stack_pointer.get());

        const auto low_byte = read(u8(IRQ_USER_TABLE_ADDRESS + 0));
        const std::uint16_t high_byte = read(u8(IRQ_USER_TABLE_ADDRESS + 1));

        program_counter.set(u16((high_byte << 8) | low_byte));

        remaining_cycles = 7;
    }
}

void Cpu6502::nmi() noexcept
{
    if (!status.is_flag_set(DisableInterrupts))
    {
        write(u16(STACK_POINTER_OFFSET + stack_pointer.get()), program_counter.high_byte());
        --(stack_pointer.get());
        write(u16(STACK_POINTER_OFFSET + stack_pointer.get()), program_counter.low_byte());
        --(stack_pointer.get());

        status.set_flag(Break, false);
        status.set_flag(Unused, true);
        status.set_flag(DisableInterrupts, true);

        write(u16(STACK_POINTER_OFFSET + stack_pointer.get()), status.get());
        --(stack_pointer.get());

        const auto low_byte = read(u8(IRQ_NMI_TABLE_ADDRESS + 0));
        const std::uint16_t high_byte = read(u8(IRQ_NMI_TABLE_ADDRESS + 1));

        program_counter.set(u16((high_byte << 8) | low_byte));

        remaining_cycles = 8;
    }
}

void Cpu6502::write(std::uint16_t address, std::uint8_t data) noexcept
{
    data_bus_.write(address, data);
}

std::uint8_t Cpu6502::read(std::uint16_t address) const noexcept
{
    return data_bus_.read(address, false);
}

uint8_t Cpu6502::fetch(uint8_t opcode, const State &state) const noexcept
{
    if (instuction_set_[opcode].address_mode != &address_mode_implied)
    { return read(state.data.address_absolute); }
    return state.data.fetched;
}
