#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <dear-imgui.hh>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <nanovg/nanovg.h>

#include <spdlog/spdlog.h>

static SDL_Window *window = nullptr;
static NVGcontext *nanovgContext = nullptr;
static imgui_bgfx_sdl::Context *imguiContext = nullptr;

static constexpr auto WINDOW_WIDTH{ 1280 };
static constexpr auto WINDOW_HEIGHT{ 720 };
static constexpr auto VIEWPORT_WIDTH{ WINDOW_HEIGHT };
static constexpr auto VIEWPORT_HEIGHT{ WINDOW_HEIGHT };

static std::function<void(NVGcontext *, int, int, float)> render_callback{ nullptr };
[[maybe_unused]] static std::function<void()> mouse_event_callback{ nullptr };
[[maybe_unused]] static std::function<void()> keyboard_event_callback{ nullptr };

namespace renderer
{
    void init(std::function<void(NVGcontext *, int, int, float)> &&render_cb) noexcept
    {
        // Initialize SDL systems
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }
        else
        {
            // Create a window
            window = SDL_CreateWindow("BGFX Tutorial",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                WINDOW_WIDTH,
                WINDOW_HEIGHT,
                SDL_WINDOW_SHOWN);
            if (window == nullptr)
            {
                spdlog::error("Failed to create window: {}", SDL_GetError());
                std::exit(EXIT_FAILURE);
            }
        }

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);
        if (!SDL_GetWindowWMInfo(window, &wmi))
        {
            spdlog::error("Failed to get native window handle: {}", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }

        bgfx::PlatformData pd;
        // and give the pointer to the window to pd
        pd.ndt = wmi.info.x11.display;
        pd.nwh = reinterpret_cast<void *>(wmi.info.x11.window);

        // Tell bgfx about the platform and window
        bgfx::setPlatformData(pd);

        bgfx::renderFrame(0);
        bgfx::Init init;
        init.type = bgfx::RendererType::Vulkan;
        init.vendorId = BGFX_PCI_ID_NONE;
        init.resolution.width = WINDOW_WIDTH;
        init.resolution.height = WINDOW_HEIGHT;
        //        init.resolution.reset = BGFX_RESET_VSYNC;
        bgfx::init(init);

        // Enable debug text.
        //        bgfx::setDebug(BGFX_DEBUG_TEXT);

        // Set view rectangle for 0th view
        bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        //        // Set view 0 clear state.
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        render_callback = std::move(render_cb);

        nanovgContext = nvgCreate(0, 0);
        imguiContext = new imgui_bgfx_sdl::Context(window, 24.f);
    }

    void run(const std::function<void(int)> &kcb) noexcept
    {
        auto &imgui = *imguiContext;
        auto &nvg = nanovgContext;
        auto &renderer = render_callback;

        auto start = std::chrono::high_resolution_clock::now();
        float elapsed = 0.f;

        bool quit = false;
        SDL_Event event;
        while (!quit)
        {
            while (SDL_PollEvent(&event) != 0)
            {
                auto handle_sdl_event = !imgui.process_event(&event);
                if (handle_sdl_event)
                {
                    if (event.type == SDL_QUIT) { quit = true; }
                    else
                        switch (event.type)
                        {
                        case SDL_KEYDOWN:
                            kcb(event.key.keysym.sym);
                            break;
                        }

                    // more events here
                }
            }

            elapsed = float(std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                                .count()) /
                      1000.f;
            start = std::chrono::high_resolution_clock::now();

            bgfx::touch(0);
            imgui.new_frame();
            nvgBeginFrame(
                nvg, static_cast<float>(VIEWPORT_WIDTH), static_cast<float>(VIEWPORT_HEIGHT), 1.f);

            renderer(nvg, WINDOW_WIDTH, WINDOW_HEIGHT, elapsed);

            nvgEndFrame(nvg);
            imgui.end_frame();

            bgfx::frame();
        }
    }

    void destroy() noexcept
    {
        delete imguiContext;
        nvgDelete(nanovgContext);
        bgfx::shutdown();
    }
} // namespace renderer
