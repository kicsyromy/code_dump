#include "platform_definitions.hh"
#include "nes_system.hh"

#include <dear-imgui.hh>
#include <fmt/format.h>

NesSystem::NesSystem() noexcept
  : cpu_{}
  , system_ram_{}
  , cartridge_{ &cpu_, &ppu_ }
  , ppu_{ cartridge_ }
  , main_bus_{ { cartridge_, CART_CPU_BUS_ADDRESS.lower_bound, CART_CPU_BUS_ADDRESS.upper_bound },
      { cpu_, 0x0000, 0x0000 },
      { system_ram_, RAM_CPU_BUS_ADDRESS.lower_bound, RAM_CPU_BUS_ADDRESS.upper_bound },
      { ppu_, PPU_CPU_BUS_ADDRESS.lower_bound, PPU_CPU_BUS_ADDRESS.upper_bound } }
{}

void NesSystem::reset() noexcept
{
    cpu_.reset();
    clock_counter_ = 0;
}

void NesSystem::clock() noexcept
{
    ppu_.clock();
    if (clock_counter_ % 3 == 0) { cpu_.clock(); }
    ++clock_counter_;
}

void NesSystem::loadCartridge(std::string_view rom_file_path) noexcept
{
    cartridge_.load(rom_file_path);
}

void NesSystem::draw_ram_content(uint16_t offset, int rows, int columns) const noexcept
{
    auto &RAM = system_ram_.RAM;

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

void NesSystem::draw_cpu_state(int width, int height) const noexcept
{
    auto &RAM = system_ram_.RAM;

    ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
    {
        ImGui::SetWindowSize("CPU", ImVec2{ float(width - height), float(height) });
        ImGui::SetWindowPos("CPU", ImVec2{ float(720), float(0) });
        ImGui::TextUnformatted("STATUS: C Z I D B U V N");
        ImGui::Text("        %d %d %d %d %d %d %d %d",
            cpu_.status.is_flag_set(Cpu6502::Carry),
            cpu_.status.is_flag_set(Cpu6502::Zero),
            !cpu_.status.is_flag_set(Cpu6502::DisableInterrupts),
            cpu_.status.is_flag_set(Cpu6502::DecimalMode),
            cpu_.status.is_flag_set(Cpu6502::Break),
            cpu_.status.is_flag_set(Cpu6502::Unused),
            cpu_.status.is_flag_set(Cpu6502::Overflow),
            cpu_.status.is_flag_set(Cpu6502::Negative));
        ImGui::TextUnformatted(fmt::format("PC: ${:06X}", cpu_.program_counter.get()).c_str());
        ImGui::TextUnformatted(fmt::format("A: ${0:04X} [ {0} ]", cpu_.accumulator.get()).c_str());
        ImGui::TextUnformatted(fmt::format("X: ${0:04X} [ {0} ]", cpu_.x.get()).c_str());
        ImGui::TextUnformatted(fmt::format("Y: ${0:04X} [ {0} ]", cpu_.y.get()).c_str());
        ImGui::TextUnformatted(fmt::format("SP: ${0:04X}", cpu_.stack_pointer.get()).c_str());
        ImGui::TextUnformatted("");

        ImGui::BeginChild("Running Instructions",
            {},
            false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
        {
            for (int i = 0; i < 100; ++i)
            {
                const auto off = [](int offset, std::size_t size) {
                    if (offset < 0) { return std::size_t(int(size) + offset); }
                    if (offset >= int(size)) { return std::size_t(offset - int(size)); }
                    return std::size_t(offset);
                }(i, RAM.size());

                const auto original_pc = cpu_.program_counter.get();
                const auto pc = std::uint16_t(cpu_.program_counter.get() + off);

                const auto opcode = main_bus_.read(pc, false);
                const auto instruction = cpu_.instruction_for_opcode(opcode);

                auto line = fmt::format("0x{:04X}: ", pc);
                line += fmt::format("{} ", instruction.name);

                auto hacked = const_cast<Cpu6502 *>(&cpu_);
                hacked->program_counter.set(u16(pc + 1));
                const auto state = instruction.address_mode(cpu_);
                hacked->program_counter.set(original_pc);

                for (std::uint16_t j = state.processed; j >= 1; --j)
                {
                    if (j == state.processed)
                    {
                        line += fmt::format(
                            std::strcmp(instruction.address_mode_name.data(), "IMM") == 0
                                ? "#${:02X}"
                                : "${:02X}",
                            main_bus_.read(u16(pc + j), false));
                    }
                    else
                    {
                        line += fmt::format("{:02X}", main_bus_.read(u16(pc + j), false));
                    }

                    if (std::strcmp(instruction.address_mode_name.data(), "REL") == 0)
                        line += fmt::format(" [${:04X}]",
                            pc + 1 + state.processed + int16_t(state.data.address_relative));
                }
                line += fmt::format(" {{{}}}", instruction.address_mode_name);

                if (pc == cpu_.program_counter.get())
                    ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f }, line.c_str());
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

void NesSystem::draw_pattern_tables(std::uint8_t palette) const noexcept
{
    static auto pattern_left =
        bgfx::createTexture2D(128, 128, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE);
    static auto pattern_right =
        bgfx::createTexture2D(128, 128, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE);

    auto table_left = ppu_.get_pattern_table(0, palette);
    auto table_right = ppu_.get_pattern_table(1, palette);

    const auto pitch = 128 * BYTES_PER_PIXEL;
    bgfx::updateTexture2D(
        pattern_left, 0, 0, 0, 0, 128, 128, bgfx::copy(table_left.data(), 128 * pitch));
    bgfx::updateTexture2D(
        pattern_right, 0, 0, 0, 0, 128, 128, bgfx::copy(table_right.data(), 128 * pitch));

    ImGui::Begin("Pattern Tables");
    ImGui::Image(pattern_left, ImVec2{ 128, 128 });
    ImGui::SameLine();
    ImGui::Image(pattern_right, ImVec2{ 128, 128 });
    ImGui::End();
}
