#include "cpu6502.hh"

#ifndef NDEBUG
#include <dear-imgui.hh>
#include <fmt/format.h>
#endif

#include "address_modes.hh"
#include "data_bus.hh"
#include "instruction_set.hh"

extern std::array<std::uint8_t, 64 * 1024> *gRAM;

Cpu6502::Cpu6502() noexcept
  : instuction_set_{ {
/* clang-format off */
        #define instr(ins, addr_mode, cycle_cnt) { #ins, &ins, #addr_mode, &addr_mode, cycle_cnt }
        instr(BRK, IMM, 7), instr(ORA, IZX, 6), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 3), instr(ORA, ZP0, 3), instr(ASL, ZP0, 5), instr(UNK, IMP, 5),
        instr(PHP, IMP, 3), instr(ORA, IMM, 2), instr(ASL, IMP, 2), instr(UNK, IMP, 2), instr(NOP, IMP, 4), instr(ORA, ABS, 4), instr(ASL, ABS, 6), instr(UNK, IMP, 6),
        instr(BPL, REL, 2), instr(ORA, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 4), instr(ORA, ZPX, 4), instr(ASL, ZPX, 6), instr(UNK, IMP, 6),
        instr(CLC, IMP, 2), instr(ORA, ABY, 4), instr(NOP, IMP, 2), instr(UNK, IMP, 7), instr(NOP, IMP, 4), instr(ORA, ABX, 4), instr(ASL, ABX, 7), instr(UNK, IMP, 7),
        instr(JSR, ABS, 6), instr(AND, IZX, 6), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(BIT, ZP0, 3), instr(AND, ZP0, 3), instr(ROL, ZP0, 5), instr(UNK, IMP, 5),
        instr(PLP, IMP, 4), instr(AND, IMM, 2), instr(ROL, IMP, 2), instr(UNK, IMP, 2), instr(BIT, ABS, 4), instr(AND, ABS, 4), instr(ROL, ABS, 6), instr(UNK, IMP, 6),
        instr(BMI, REL, 2), instr(AND, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 4), instr(AND, ZPX, 4), instr(ROL, ZPX, 6), instr(UNK, IMP, 6),
        instr(SEC, IMP, 2), instr(AND, ABY, 4), instr(NOP, IMP, 2), instr(UNK, IMP, 7), instr(NOP, IMP, 4), instr(AND, ABX, 4), instr(ROL, ABX, 7), instr(UNK, IMP, 7),
        instr(RTI, IMP, 6), instr(EOR, IZX, 6), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 3), instr(EOR, ZP0, 3), instr(LSR, ZP0, 5), instr(UNK, IMP, 5),
        instr(PHA, IMP, 3), instr(EOR, IMM, 2), instr(LSR, IMP, 2), instr(UNK, IMP, 2), instr(JMP, ABS, 3), instr(EOR, ABS, 4), instr(LSR, ABS, 6), instr(UNK, IMP, 6),
        instr(BVC, REL, 2), instr(EOR, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 4), instr(EOR, ZPX, 4), instr(LSR, ZPX, 6), instr(UNK, IMP, 6),
        instr(CLI, IMP, 2), instr(EOR, ABY, 4), instr(NOP, IMP, 2), instr(UNK, IMP, 7), instr(NOP, IMP, 4), instr(EOR, ABX, 4), instr(LSR, ABX, 7), instr(UNK, IMP, 7),
        instr(RTS, IMP, 6), instr(ADC, IZX, 6), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 3), instr(ADC, ZP0, 3), instr(ROR, ZP0, 5), instr(UNK, IMP, 5),
        instr(PLA, IMP, 4), instr(ADC, IMM, 2), instr(ROR, IMP, 2), instr(UNK, IMP, 2), instr(JMP, IND, 5), instr(ADC, ABS, 4), instr(ROR, ABS, 6), instr(UNK, IMP, 6),
        instr(BVS, REL, 2), instr(ADC, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 4), instr(ADC, ZPX, 4), instr(ROR, ZPX, 6), instr(UNK, IMP, 6),
        instr(SEI, IMP, 2), instr(ADC, ABY, 4), instr(NOP, IMP, 2), instr(UNK, IMP, 7), instr(NOP, IMP, 4), instr(ADC, ABX, 4), instr(ROR, ABX, 7), instr(UNK, IMP, 7),
        instr(NOP, IMP, 2), instr(STA, IZX, 6), instr(NOP, IMP, 2), instr(UNK, IMP, 6), instr(STY, ZP0, 3), instr(STA, ZP0, 3), instr(STX, ZP0, 3), instr(UNK, IMP, 3),
        instr(DEY, IMP, 2), instr(NOP, IMP, 2), instr(TXA, IMP, 2), instr(UNK, IMP, 2), instr(STY, ABS, 4), instr(STA, ABS, 4), instr(STX, ABS, 4), instr(UNK, IMP, 4),
        instr(BCC, REL, 2), instr(STA, IZY, 6), instr(UNK, IMP, 2), instr(UNK, IMP, 6), instr(STY, ZPX, 4), instr(STA, ZPX, 4), instr(STX, ZPY, 4), instr(UNK, IMP, 4),
        instr(TYA, IMP, 2), instr(STA, ABY, 5), instr(TXS, IMP, 2), instr(UNK, IMP, 5), instr(NOP, IMP, 5), instr(STA, ABX, 5), instr(UNK, IMP, 5), instr(UNK, IMP, 5),
        instr(LDY, IMM, 2), instr(LDA, IZX, 6), instr(LDX, IMM, 2), instr(UNK, IMP, 6), instr(LDY, ZP0, 3), instr(LDA, ZP0, 3), instr(LDX, ZP0, 3), instr(UNK, IMP, 3),
        instr(TAY, IMP, 2), instr(LDA, IMM, 2), instr(TAX, IMP, 2), instr(UNK, IMP, 2), instr(LDY, ABS, 4), instr(LDA, ABS, 4), instr(LDX, ABS, 4), instr(UNK, IMP, 4),
        instr(BCS, REL, 2), instr(LDA, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 5), instr(LDY, ZPX, 4), instr(LDA, ZPX, 4), instr(LDX, ZPY, 4), instr(UNK, IMP, 4),
        instr(CLV, IMP, 2), instr(LDA, ABY, 4), instr(TSX, IMP, 2), instr(UNK, IMP, 4), instr(LDY, ABX, 4), instr(LDA, ABX, 4), instr(LDX, ABY, 4), instr(UNK, IMP, 4),
        instr(CPY, IMM, 2), instr(CMP, IZX, 6), instr(NOP, IMP, 2), instr(UNK, IMP, 8), instr(CPY, ZP0, 3), instr(CMP, ZP0, 3), instr(DEC, ZP0, 5), instr(UNK, IMP, 5),
        instr(INY, IMP, 2), instr(CMP, IMM, 2), instr(DEX, IMP, 2), instr(UNK, IMP, 2), instr(CPY, ABS, 4), instr(CMP, ABS, 4), instr(DEC, ABS, 6), instr(UNK, IMP, 6),
        instr(BNE, REL, 2), instr(CMP, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 4), instr(CMP, ZPX, 4), instr(DEC, ZPX, 6), instr(UNK, IMP, 6),
        instr(CLD, IMP, 2), instr(CMP, ABY, 4), instr(NOP, IMP, 2), instr(UNK, IMP, 7), instr(NOP, IMP, 4), instr(CMP, ABX, 4), instr(DEC, ABX, 7), instr(UNK, IMP, 7),
        instr(CPX, IMM, 2), instr(SBC, IZX, 6), instr(NOP, IMP, 2), instr(UNK, IMP, 8), instr(CPX, ZP0, 3), instr(SBC, ZP0, 3), instr(INC, ZP0, 5), instr(UNK, IMP, 5),
        instr(INX, IMP, 2), instr(SBC, IMM, 2), instr(NOP, IMP, 2), instr(SBC, IMP, 2), instr(CPX, ABS, 4), instr(SBC, ABS, 4), instr(INC, ABS, 6), instr(UNK, IMP, 6),
        instr(BEQ, REL, 2), instr(SBC, IZY, 5), instr(UNK, IMP, 2), instr(UNK, IMP, 8), instr(NOP, IMP, 4), instr(SBC, ZPX, 4), instr(INC, ZPX, 6), instr(UNK, IMP, 6),
        instr(SED, IMP, 2), instr(SBC, ABY, 4), instr(NOP, IMP, 2), instr(UNK, IMP, 7), instr(NOP, IMP, 4), instr(SBC, ABX, 4), instr(INC, ABX, 7), instr(UNK, IMP, 7),
        /* clang-format on */
    } }
{}

Cpu6502::~Cpu6502() noexcept = default;

#ifndef NDEBUG
void Cpu6502::draw_ram_content(uint16_t offset, int rows, int columns) const noexcept
{
    auto &RAM = *gRAM;

    ImGui::Begin("RAM");
    //    ImGui::SetWindowFontScale(0.9f);
    auto mem = RAM[offset];
    for (int i = 0; i < columns; ++i)
    {
        std::string line = fmt::format("${:06X}: ", offset + i * columns);
        for (int j = 0; j < rows; ++j)
        {
            mem = RAM[size_t(offset + (columns * i) + j)];
            line += fmt::format("{:02x} ", mem);
        }
        ImGui::TextUnformatted(line.c_str());
    }
    ImGui::TextUnformatted("");
    ImGui::End();
}

void Cpu6502::draw_cpu_state(int width, int height) const noexcept
{
    auto &RAM = *gRAM;

    ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
    {
        ImGui::SetWindowSize("CPU", ImVec2{ float(width - height), float(height) });
        ImGui::SetWindowPos("CPU", ImVec2{ float(720), float(0) });
        ImGui::TextUnformatted("STATUS: C Z I D B U V N");
        ImGui::Text("        %d %d %d %d %d %d %d %d",
            status.is_flag_set(Cpu6502::Carry),
            status.is_flag_set(Cpu6502::Zero),
            !status.is_flag_set(Cpu6502::DisableInterrupts),
            status.is_flag_set(Cpu6502::DecimalMode),
            status.is_flag_set(Cpu6502::Break),
            status.is_flag_set(Cpu6502::Unused),
            status.is_flag_set(Cpu6502::Overflow),
            status.is_flag_set(Cpu6502::Negative));
        ImGui::TextUnformatted(fmt::format("PC: ${:06X}", program_counter.get()).c_str());
        ImGui::TextUnformatted(fmt::format("A: ${0:04X} [ {0} ]", accumulator.get()).c_str());
        ImGui::TextUnformatted(fmt::format("X: ${0:04X} [ {0} ]", x.get()).c_str());
        ImGui::TextUnformatted(fmt::format("Y: ${0:04X} [ {0} ]", y.get()).c_str());
        ImGui::TextUnformatted(fmt::format("SP: ${0:04X}", stack_pointer.get()).c_str());
        ImGui::TextUnformatted("");

        ImGui::BeginChild("Running Instructions");
        {
            for (int i = 0; i < 100; ++i)
            {
                const auto off = [](int offset, std::size_t size) {
                    if (offset < 0) { return std::size_t(int(size) + offset); }
                    if (offset >= int(size)) { return std::size_t(offset - int(size)); }
                    return std::size_t(offset);
                }(i, RAM.size());
                const auto pc = u16(program_counter.get() + off);
                const auto opcode = read(pc);
                const auto &instruction = instuction_set_[opcode];

                auto line = fmt::format("{:#06X}: ", pc);
                line += fmt::format("{} ", instruction.name);

                const auto state = instruction.address_mode(*this);
                for (std::uint16_t j = 1; j <= state.processed; ++j)
                {
                    if (j == 1)
                    {
                        line += fmt::format(instruction.address_mode == &address_mode_immediate
                                                ? "#${:02X}"
                                                : "${:02X}",
                            read(u16(pc + j)));
                    }
                    else
                    {
                        line += fmt::format("${:02X}", read(u16(pc + j)));
                    }
                }
                line += fmt::format(" {{{}}}", instruction.address_mode_name);

                if (pc == program_counter.get())
                { ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f }, line.c_str()); }
                else
                {
                    ImGui::TextUnformatted(line.c_str());
                }

                i += state.processed;
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

#endif

void Cpu6502::connect(data_bus_t bus, BusReadFunction &&r, BusWriteFunction &&w) noexcept
{
    data_bus_ = bus;
    bus_read_ = r;
    bus_write_ = w;
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

void Cpu6502::clock() noexcept
{
    if (remaining_cycles == 0)
    {
        const auto opcode = read(program_counter.get());

        status.set_flag(Unused, true);

        ++(program_counter.get());

        const auto &instruction = instuction_set_[opcode];

        const auto state = instruction.address_mode(*this);
        const auto visitor = [this, opcode, &state](const auto &f) {
            using inst_t = Instruction<Cpu6502>;
            if constexpr (std::is_same_v<std::decay_t<decltype(f)>, inst_t::instruction_f>)
                return f(*this, opcode, state);
            if constexpr (std::is_same_v<std::decay_t<decltype(f)>, inst_t::instruction_no_opcode>)
                return f(*this, state);
            if constexpr (std::is_same_v<std::decay_t<decltype(f)>, inst_t::instruction_no_params>)
                return f(*this);
        };
        const auto additional_cycles = std::visit(visitor, instruction.operation);
        remaining_cycles = u8(instuction_set_[opcode].cycles + additional_cycles);
        status.set_flag(Unused, true);
    }

    remaining_cycles--;
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

        const auto low_byte = read(u8(USER_IRQ_VECTOR_ADDRESS + 0));
        const std::uint16_t high_byte = read(u8(USER_IRQ_VECTOR_ADDRESS + 1));

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

        const auto low_byte = read(u8(NMI_IRQ_VECTOR_ADDRESS + 0));
        const std::uint16_t high_byte = read(u8(NMI_IRQ_VECTOR_ADDRESS + 1));

        program_counter.set(u16((high_byte << 8) | low_byte));

        remaining_cycles = 8;
    }
}

uint8_t Cpu6502::read(uint16_t address) const noexcept
{
    if (data_bus_ != nullptr) { return bus_read_(address, false, data_bus_); }
    return 0;
}

void Cpu6502::write(uint16_t address, uint8_t data) noexcept
{
    if (data_bus_ != nullptr) { bus_write_(address, data, data_bus_); }
}
