#include "cpu6502.hh"
#include "data_bus.hh"
#include "renderer.hh"

#include <cstdint>
#include <sstream>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <dear-imgui.hh>
#include <nanovg/nanovg.h>

#include <spdlog/spdlog.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
    bool my_tool_active = true;
    float my_color[4];

    DataBus bus{};
    Cpu6502 cpu{ bus };

    std::stringstream program_data;
    program_data
        << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
    std::uint16_t offset{ 0x8000 };
    while (!program_data.eof())
    {
        std::string byte;
        program_data >> byte;
        bus.RAM[offset++] = u8(std::stoul(byte, nullptr, 16));
    }

    bus.RAM[Cpu6502::RESET_ADDRESS_LOCATION] = 0x00;
    bus.RAM[Cpu6502::RESET_ADDRESS_LOCATION + 1] = 0x80;

    cpu.reset();

    renderer::init([&my_tool_active, &my_color, &cpu](NVGcontext *nvg, int width, int height) {
        nvgBeginPath(nvg);
        nvgRect(nvg, 0, 0, float(width), float(height));
        nvgFillColor(nvg, nvgRGBA(0, 0, 0, 255));
        nvgFill(nvg);

        //        ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
        //        if (ImGui::BeginMenuBar())
        //        {
        //            if (ImGui::BeginMenu("File"))
        //            {
        //                if (ImGui::MenuItem("Open..", "Ctrl+O"))
        //                { /* Do stuff */
        //                }
        //                if (ImGui::MenuItem("Save", "Ctrl+S"))
        //                { /* Do stuff */
        //                }
        //                if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
        //                ImGui::EndMenu();
        //            }
        //            ImGui::EndMenuBar();
        //        }

        //        // Edit a color (stored as ~4 floats)
        //        ImGui::ColorEdit4("Color", my_color);

        //        // Plot some values
        //        const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
        //        ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

        //        // Display contents in a scrolling region
        //        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        //        ImGui::BeginChild("Scrolling");
        //        for (int n = 0; n < 50; n++) ImGui::Text("%04d: Some text", n);
        //        ImGui::EndChild();
        //        ImGui::End();

        // Use debug font to print information about this example.
        //        bgfx::dbgTextClear();

        //        const auto *stats = bgfx::getStats();
        //        bgfx::dbgTextPrintf(0,
        //            0,
        //            0x0f,
        //            "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
        //            stats->width,
        //            stats->height,
        //            stats->textWidth,
        //            stats->textHeight);
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
