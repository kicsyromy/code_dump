#include "cpu6502.hh"
#include "data_bus.hh"
#include "renderer.hh"
#include "ram.hh"

#include <cstdint>
#include <sstream>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <dear-imgui.hh>
#include <nanovg/nanovg.h>

#include <spdlog/spdlog.h>

std::array<std::uint8_t, 64 * 1024> *gRAM;

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
    bool my_tool_active = true;
    float my_color[4];

    Cpu6502 cpu{};
    Ram memory{};
    DataBus<Cpu6502, Ram> bus{ { cpu, 0x0000, 0x0000 }, { memory, 0x0000, 0xFFFF } };
    gRAM = &memory.RAM;

    std::stringstream program_data;
    program_data
        << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
    std::uint16_t offset{ 0x8000 };
    while (!program_data.eof())
    {
        std::string byte;
        program_data >> byte;
        memory.write_to(offset++, u8(std::stoul(byte, nullptr, 16)));
    }

    memory.write_to(Cpu6502::RESET_ADDRESS_LOCATION, u8(0x00));
    memory.write_to(Cpu6502::RESET_ADDRESS_LOCATION + 1, u8(0x80));

    cpu.reset();

    renderer::init([&my_tool_active, &my_color, &cpu](NVGcontext *, int width, int height) {
        ImGui::PushFont(ImGui::Font::Mono);
        cpu.draw_ram_content(0, 16, 16);
        cpu.draw_ram_content(0x8000, 16, 16);
        cpu.draw_cpu_state(width, height);
        ImGui::PopFont();
    });

    renderer::run([&](int keycode) {
        switch (keycode)
        {
        case SDLK_SPACE:
            cpu.remaining_cycles = 0;
            cpu.clock();
            break;
        case SDLK_r:
            cpu.reset();
            break;
        }
    });
    renderer::destroy();

    return 0;
}
