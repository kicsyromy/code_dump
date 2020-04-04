#include "renderer.hh"
#include "register.hh"

#include <cstdint>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <dear-imgui.hh>
#include <nanovg/nanovg.h>

#include <spdlog/spdlog.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
    bool my_tool_active = true;
    float my_color[4];

    renderer::init([&my_tool_active, &my_color](NVGcontext *nvg) {
        nvgBeginPath(nvg);
        nvgRect(nvg, 100, 100, 120, 30);
        nvgCircle(nvg, 120, 120, 5);
        nvgPathWinding(nvg, NVG_HOLE); // Mark circle as a hole.
        nvgFillColor(nvg, nvgRGBA(255, 192, 0, 255));
        nvgFill(nvg);

        ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O"))
                { /* Do stuff */
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                { /* Do stuff */
                }
                if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Edit a color (stored as ~4 floats)
        ImGui::ColorEdit4("Color", my_color);

        // Plot some values
        const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
        ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        ImGui::BeginChild("Scrolling");
        for (int n = 0; n < 50; n++) ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        ImGui::End();

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();

        bgfx::dbgTextPrintf(0,
            1,
            0x0f,
            "Color can be changed with ANSI "
            "\x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

        bgfx::dbgTextPrintf(80,
            1,
            0x0f,
            "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m "
            "   \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(80,
            2,
            0x0f,
            "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m "
            "   \x1b[;15m    \x1b[0m");

        const bgfx::Stats *stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0,
            2,
            0x0f,
            "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
            stats->width,
            stats->height,
            stats->textWidth,
            stats->textHeight);
    });

    renderer::run();
    renderer::destroy();

    return 0;
}
