#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <dear-imgui.hh>
#include <nanovg/nanovg.h>
#include <spdlog/spdlog.h>

#include <cstdint>

SDL_Window *window = NULL;
const int WIDTH = 800;
const int HEIGHT = 600;

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
    // Use the default logger (stdout, multi-threaded, colored)
    spdlog::info("Hello, {}!", "World");

    fmt::print("Hello, from {}\n", "{fmt}");

    // Initialize SDL systems
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    { printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError()); }
    else
    {
        // Create a window
        window = SDL_CreateWindow("BGFX Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_SHOWN);
        if (window == NULL)
        { printf("Window could not be created! SDL_Error: %s\n", SDL_GetError()); }
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) { return 1; }

    bgfx::PlatformData pd;
    // and give the pointer to the window to pd
    pd.ndt = wmi.info.x11.display;
    pd.nwh = reinterpret_cast<void *>(wmi.info.x11.window);

    // Tell bgfx about the platform and window
    bgfx::setPlatformData(pd);

    bgfx::Init init;
    init.type = bgfx::RendererType::Vulkan;
    init.vendorId = BGFX_PCI_ID_INTEL;
    init.resolution.width = WIDTH;
    init.resolution.height = HEIGHT;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view rectangle for 0th view
    bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);

    auto vg = nvgCreate(1, 0);
    {
        auto imgui = imgui_bgfx_sdl::Context(window);

        // Poll for events and wait till user closes window
        bool quit = false;
        SDL_Event event;
        bool my_tool_active = true;
        while (!quit)
        {
            while (SDL_PollEvent(&event) != 0)
            {
                imgui.process_event(&event);
                if (event.type == SDL_QUIT) { quit = true; }
            }

            imgui.new_frame();

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

            float my_color[4];
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

            imgui.end_frame();

            nvgBeginFrame(vg, float(WIDTH), float(HEIGHT), 1.0f);

            nvgBeginPath(vg);
            nvgRect(vg, 100, 100, 120, 30);
            nvgCircle(vg, 120, 120, 5);
            nvgPathWinding(vg, NVG_HOLE); // Mark circle as a hole.
            nvgFillColor(vg, nvgRGBA(255, 192, 0, 255));
            nvgFill(vg);

            nvgEndFrame(vg);

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
                "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    "
                "\x1b[; 6m    \x1b[; 7m    \x1b[0m");
            bgfx::dbgTextPrintf(80,
                2,
                0x0f,
                "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    "
                "\x1b[;14m    \x1b[;15m    \x1b[0m");

            const bgfx::Stats *stats = bgfx::getStats();
            bgfx::dbgTextPrintf(0,
                2,
                0x0f,
                "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
                stats->width,
                stats->height,
                stats->textWidth,
                stats->textHeight);

            // Advance to next frame. Rendering thread will be kicked to
            // process submitted rendering primitives.
            bgfx::frame();
        }
    }

    nvgDelete(vg);

    // Shutdown bgfx.
    bgfx::shutdown();
}
