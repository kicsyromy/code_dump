#include "renderer.hh"
#include "register.hh"
#include "fps.hh"

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
    fps_counter counter{};
    counter.update();

    renderer::init([&my_tool_active, &my_color, &counter](NVGcontext *nvg) {
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

        bgfx::dbgTextPrintf(0, 1, 0x0f, fmt::format("FPS: {}", counter.get()).c_str());

        const auto *stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0,
            2,
            0x0f,
            "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
            stats->width,
            stats->height,
            stats->textWidth,
            stats->textHeight);

        counter.update();
    });

    renderer::run();
    renderer::destroy();

    return 0;
}
