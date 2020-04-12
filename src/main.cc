#include "nes_system.hh"
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
    NesSystem nes;
    nes.loadCartridge(SOURCE_DIR "/nestest.nes");
    nes.reset();

    bool running = false;

    float residual_time{ 0.f };
    int framebuffer = -1;
    NVGpaint nvg_pattern{};

    std::uint8_t selected_palette = 0;

    renderer::init([&](NVGcontext *nvg, int width, int height, float elapsed) {
        if (running)
        {
            if (residual_time > 0.f)
                residual_time -= elapsed;
            else
            {
                residual_time += (1.f / 60.f) - elapsed;
                do
                {
                    nes.clock();
                } while (!nes.ppu().frame_complete());
            }
        }

        if (framebuffer == -1)
        {
            framebuffer = nvgCreateImageRGBA(nvg, 256, 240, 0, nes.ppu().framebuffer().data());
            nvg_pattern = nvgImagePattern(nvg, 0, 0, 256, 240, 0.f, framebuffer, 1.f);
        }

        nvgUpdateImage(nvg, framebuffer, nes.ppu().framebuffer().data());

        nvgScale(nvg, 3.f, 3.f);
        nvgBeginPath(nvg);
        nvgRect(nvg, 0, 0, 256, 240);
        nvgFillPaint(nvg, nvg_pattern);
        nvgFill(nvg);

        static_cast<void>(width);
        static_cast<void>(height);

        ImGui::PushFont(ImGui::Font::Mono);
        // nes.draw_ram_content(0, 16, 16);
        // nes.draw_ram_content(0x8000, 16, 16);
        nes.draw_cpu_state(width, height);
        nes.draw_pattern_tables(selected_palette);
        ImGui::PopFont();
    });

    renderer::run([&](int keycode) {
        switch (keycode)
        {
        case SDLK_SPACE:
            running = !running;
            break;
        case SDLK_r:
            nes.reset();
            break;
        case SDLK_c: {
            do
            {
                nes.clock();
            } while (!nes.cpu().cycle_complete());
            do
            {
                nes.clock();
            } while (nes.cpu().cycle_complete());
        }
        break;
        case SDLK_f:
        {
            do
            {
                nes.clock();
            } while (!nes.ppu().frame_complete());
            do
            {
                nes.clock();
            } while (!nes.cpu().cycle_complete());
            break;
        }
        case SDLK_p:
            const auto new_pal = selected_palette + 1;
            selected_palette = std::uint8_t(new_pal & 0x07);
            break;
        }
    });
    renderer::destroy();

    return 0;
}
