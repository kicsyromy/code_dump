#include "core/voot_application.hh"

#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"
#include "events/voot_render_event.hh"
#include "events/voot_window_events.hh"

#include <SDL2.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <variant>
#include <unordered_set>

#ifdef __linux__
#ifdef SDL_VIDEO_DRIVER_WAYLAND
const std::string VT_WAYLAND_DISPLAY = [] {
    const char *env = std::getenv("WAYLAND_DISPLAY");

    if (env == nullptr)
        return "";

    return env;
}();
#else
const std::string VT_WAYLAND_DISPLAY{ "" };
#endif
#endif

namespace
{
    voot::Application *g_app_instance = nullptr;

    const std::uint32_t USER_EVENT_TYPE{ [] {
        return SDL_RegisterEvents(1);
    }() };

    /* TODO: Figure out if we want a separate rendering thread, disable for now */
    //    const auto NO_OFFSCREEN_RENDERER = []() -> int {
    //        bgfx::renderFrame(0);
    //        return 0;
    //    }();

    struct EventWrapper
    {
        EventWrapper(voot::Event *e) noexcept
          : event{ std::unique_ptr<voot::Event>(e) }
        {}

        EventWrapper(const std::shared_ptr<voot::Event> &e) noexcept
          : event{ e }
        {}

        std::variant<std::unique_ptr<voot::Event>, std::shared_ptr<voot::Event>> event;
    };

    /* TODO: Figure out how to make the a function */
#define CREATE_WINDOW_EVENT(event_data, window_event)                                            \
    voot::WindowEventVariant window_event_var;                                                   \
    do                                                                                           \
    {                                                                                            \
        using namespace voot;                                                                    \
        switch ((event_data).event)                                                              \
        {                                                                                        \
        default:                                                                                 \
            break;                                                                               \
        case SDL_WINDOWEVENT_SHOWN:                                                              \
        case SDL_WINDOWEVENT_EXPOSED:                                                            \
            window_event_var = voot::WindowShowEvent{};                                          \
            (window_event) = &(std::get<voot::WindowShowEvent>(window_event_var));               \
            break;                                                                               \
        case SDL_WINDOWEVENT_HIDDEN:                                                             \
            window_event_var = voot::WindowHideEvent{};                                          \
            (window_event) = &(std::get<voot::WindowHideEvent>(window_event_var));               \
            break;                                                                               \
        case SDL_WINDOWEVENT_MOVED:                                                              \
            window_event_var = voot::WindowMoveEvent{ (event_data).data1, (event_data).data2 };  \
            (window_event) = &(std::get<voot::WindowMoveEvent>(window_event_var));               \
            break;                                                                               \
        case SDL_WINDOWEVENT_RESIZED:                                                            \
        case SDL_WINDOWEVENT_SIZE_CHANGED:                                                       \
            window_event_var = voot::WindowResizeEvent{ static_cast<size_t>((event_data).data1), \
                static_cast<size_t>((event_data).data2) };                                       \
            (window_event) = &(std::get<voot::WindowResizeEvent>(window_event_var));             \
            break;                                                                               \
        case SDL_WINDOWEVENT_MINIMIZED:                                                          \
        case SDL_WINDOWEVENT_MAXIMIZED:                                                          \
        case SDL_WINDOWEVENT_RESTORED:                                                           \
        case SDL_WINDOWEVENT_ENTER:                                                              \
        case SDL_WINDOWEVENT_LEAVE:                                                              \
            break;                                                                               \
        case SDL_WINDOWEVENT_FOCUS_GAINED:                                                       \
            window_event_var = voot::WindowGainFocusEvent{};                                     \
            (window_event) = &(std::get<voot::WindowGainFocusEvent>(window_event_var));          \
            break;                                                                               \
        case SDL_WINDOWEVENT_FOCUS_LOST:                                                         \
            window_event_var = voot::WindowLooseFocusEvent{};                                    \
            (window_event) = &(std::get<voot::WindowLooseFocusEvent>(window_event_var));         \
            break;                                                                               \
        case SDL_WINDOWEVENT_CLOSE:                                                              \
            window_event_var = voot::WindowCloseEvent{};                                         \
            (window_event) = &(std::get<voot::WindowCloseEvent>(window_event_var));              \
            break;                                                                               \
        case SDL_WINDOWEVENT_TAKE_FOCUS:                                                         \
        case SDL_WINDOWEVENT_HIT_TEST:                                                           \
            break;                                                                               \
        }                                                                                        \
    } while (false)
} // namespace

VOOT_BEGIN_NAMESPACE

Application *Application::instance() noexcept
{
    return g_app_instance;
}

Application::Application() noexcept
{
    if (g_app_instance != nullptr)
    {
        VT_LOG_FATAL("Cannot create multiple instances of Application in one process");
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        VT_LOG_FATAL("Failed to initialize SDL: {}", SDL_GetError());
    }

    bgfx_platfrom_window_ =
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>{ SDL_CreateWindow("",
                                                                       SDL_WINDOWPOS_UNDEFINED,
                                                                       SDL_WINDOWPOS_UNDEFINED,
                                                                       0,
                                                                       0,
                                                                       SDL_WINDOW_HIDDEN),
            &SDL_DestroyWindow };

    if (bgfx_platfrom_window_ == nullptr)
    {
        VT_LOG_FATAL("Failed to create platform render window: {}", SDL_GetError());
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(bgfx_platfrom_window_.get(), &wmi))
    {
        std::exit(EXIT_FAILURE);
    }

    bgfx::PlatformData pd;
    std::memset(static_cast<void *>(&pd), 0, sizeof(pd));
#ifdef _WIN32
    pd.ndt = wmi.info.win.hdc;
    pd.nwh = reinterpret_cast<void *>(wmi.info.win.window);
#else
    if (!VT_WAYLAND_DISPLAY.empty())
    {
        pd.ndt = wmi.info.wl.display;
        pd.nwh = wmi.info.wl.surface;
    }
    else /* X11 */
    {
        pd.ndt = wmi.info.x11.display;
        pd.nwh = reinterpret_cast<void *>(wmi.info.x11.window);
    }
#endif

    bgfx::setPlatformData(pd);

    bgfx::Init init;
#ifdef _WIN32
    init.type = bgfx::RendererType::Direct3D11;
#else
    init.type = bgfx::RendererType::OpenGL;
#endif
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = 1;
    init.resolution.height = 1;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    //    const auto *caps = bgfx::getCaps();
    //    if ((caps->supported & (BGFX_CAPS_SWAP_CHAIN)) != 0)
    //    {
    //        bgfx::shutdown();

    //#ifdef __linux__
    //        VT_LOG_WARN("Swap chain extension not present on Vulkan, falling back to OpenGL
    //        rendering");

    //        init = {};
    //        init.type = bgfx::RendererType::OpenGL;
    //        init.vendorId = BGFX_PCI_ID_NONE;
    //        init.resolution.width = 1;
    //        init.resolution.height = 1;
    //        init.resolution.reset = BGFX_RESET_VSYNC;
    //        bgfx::init(init);
    //#else
    //        VT_LOG_FATAL("Swap chain extension not supported, bailing out...");
    //#endif
    //    }

    g_app_instance = this;
}

Application::~Application() noexcept
{
    g_app_instance = nullptr;
    bgfx::shutdown();
    SDL_Quit();
}

void Application::post_event(gsl::owner<Event *> event, int receiver_id) noexcept
{
    VT_LOG_DEBUG("Pushing owned event to queue: {}", event->event_name());

    static_cast<void>(receiver_id);

    SDL_Event sdl_event;
    SDL_zero(sdl_event);

    sdl_event.type = USER_EVENT_TYPE;
    sdl_event.user.code = 0;
    sdl_event.user.data1 = new EventWrapper{ event };
    sdl_event.user.data2 = nullptr;

    SDL_PushEvent(&sdl_event);
}

void Application::post_event(const std::shared_ptr<Event> &event, int receiver_id) noexcept
{
    VT_LOG_DEBUG("Pushing event to queue: {}", event->event_name());

    static_cast<void>(receiver_id);

    SDL_Event sdl_event;
    SDL_zero(sdl_event);

    sdl_event.type = USER_EVENT_TYPE;
    sdl_event.user.code = 0;
    sdl_event.user.data1 = new EventWrapper{ event };
    sdl_event.user.data2 = nullptr;

    SDL_PushEvent(&sdl_event);
}

void Application::register_event_handler(EventType type,
    EventCallback callback,
    void *user_data,
    int user_id,
    int priority) noexcept
{
    auto &event_clients = gsl::at(clients_, std::size_t(type));

    event_clients.push_back({ user_id, priority, callback, user_data });
    std::sort(event_clients.begin(),
        event_clients.end(),
        [](const EventClient &c1, const EventClient &c2) noexcept -> bool {
            return c1.priority < c2.priority;
        });
}

void Application::exec()
{
    std::unordered_set<std::uint32_t> open_windows;

    auto start = std::chrono::high_resolution_clock::now();
    SDL_Event event;
    while (!quit_)
    {
        using namespace voot;

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit_ = true;
            }
            else
            {
                if (event.type == USER_EVENT_TYPE)
                {
                    auto *wrapped_event = static_cast<EventWrapper *>(event.user.data1);

                    const auto visitor = [](auto &e) -> Event * {
                        return e.get();
                    };
                    auto *e = std::visit(visitor, wrapped_event->event);
                    VT_LOG_DEBUG("Poping synthetic event: {}", e->event_name());

                    for (auto &client : clients_[static_cast<std::size_t>(EventType::User)])
                    {
                        if (!client.callback(client.id, e, client.callback_data))
                            break;
                    }

                    delete wrapped_event;

                    continue;
                }

                switch (event.type)
                {
                case SDL_WINDOWEVENT: {
                    voot::Event *window_event = nullptr;
                    CREATE_WINDOW_EVENT(event.window, window_event);

                    if (window_event != nullptr)
                    {
                        const auto window_id = event.window.windowID;
                        VT_LOG_DEBUG("New window event: {}", window_event->event_name());

                        auto &window_clients =
                            gsl::at(clients_, static_cast<std::size_t>(window_event->event_type()));
                        for (auto &client : window_clients)
                        {
                            if (client.callback != nullptr &&
                                (client.id == -1 ||
                                    (client.id >= 0 && std::uint32_t(client.id) == window_id)))
                            {
                                if (!client.callback(client.id, window_event, client.callback_data))
                                    break;
                            }
                        }

                        if (window_event->event_type() == EventType::WindowShown)
                        {
                            if (open_windows.count(window_id) == 0)
                            {
                                open_windows.insert(window_id);
                            }
                        }
                        else if (window_event->event_type() == EventType::WindowClosed)
                        {
                            open_windows.erase(window_id);
                            if (open_windows.empty())
                                quit_ = true;
                        }
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    MouseMoveEvent mouse_move_event{ event.motion.x, event.motion.y };
                    VT_LOG_DEBUG("Mouse moved: X: {}, Y: {}",
                        mouse_move_event.coordinates().first,
                        mouse_move_event.coordinates().second);

                    const auto window_id = event.motion.windowID;

                    auto &mouse_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(MouseMoveEvent::event_type()));
                    for (auto &client : mouse_event_clients)
                    {
                        if (client.callback != nullptr &&
                            (client.id == -1 ||
                                (client.id >= 0 && std::uint32_t(client.id) == window_id)))
                        {
                            if (!client.callback(client.id,
                                    &mouse_move_event,
                                    client.callback_data))
                                break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    const auto mouse_button = static_cast<MouseButton>(event.button.button - 1);
                    gsl::at(mouse_button_states_, std::size_t(mouse_button)) = 1;

                    const auto window_id = event.button.windowID;

                    MouseButtonPressEvent mouse_press_event{ event.button.x,
                        event.button.y,
                        mouse_button };
                    VT_LOG_DEBUG("Mouse button pressed: B: {}, X: {}, Y: {}",
                        static_cast<int>(mouse_press_event.button()),
                        mouse_press_event.coordinates().first,
                        mouse_press_event.coordinates().second);

                    auto &mouse_event_clients = gsl::at(clients_,
                        static_cast<std::size_t>(MouseButtonPressEvent::event_type()));
                    for (auto &client : mouse_event_clients)
                    {
                        if (client.callback != nullptr &&
                            (client.id == -1 ||
                                (client.id >= 0 && std::uint32_t(client.id) == window_id)))
                        {
                            if (!client.callback(client.id,
                                    &mouse_press_event,
                                    client.callback_data))
                                break;
                        }
                    }

                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    const auto mouse_button = static_cast<MouseButton>(event.button.button - 1);
                    gsl::at(mouse_button_states_, std::size_t(mouse_button)) = 0;

                    const auto window_id = event.button.windowID;

                    MouseButtonReleaseEvent mouse_release_event{ event.button.x,
                        event.button.y,
                        mouse_button };
                    VT_LOG_DEBUG("Mouse button released: B: {}, X: {}, Y: {}",
                        static_cast<int>(mouse_release_event.button()),
                        mouse_release_event.coordinates().first,
                        mouse_release_event.coordinates().second);

                    auto &mouse_event_clients = gsl::at(clients_,
                        static_cast<std::size_t>(MouseButtonReleaseEvent::event_type()));
                    for (auto &client : mouse_event_clients)
                    {
                        if (client.callback != nullptr &&
                            (client.id == -1 ||
                                (client.id >= 0 && std::uint32_t(client.id) == window_id)))
                        {
                            if (!client.callback(client.id,
                                    &mouse_release_event,
                                    client.callback_data))
                                break;
                        }
                    }

                    break;
                }
                case SDL_KEYUP: {
                    const auto key = event.key.keysym.scancode;
                    gsl::at(key_states_, std::size_t(key)) = 0;

                    const auto window_id = event.key.windowID;

                    KeyReleaseEvent key_event{ KeyCode(key) };
                    VT_LOG_DEBUG("Key released: {}", key);

                    auto &key_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(KeyReleaseEvent::event_type()));
                    for (auto &client : key_event_clients)
                    {
                        if (client.callback != nullptr &&
                            (client.id == -1 ||
                                (client.id >= 0 && std::uint32_t(client.id) == window_id)))
                        {
                            if (!client.callback(client.id, &key_event, client.callback_data))
                                break;
                        }
                    }

                    break;
                }
                case SDL_KEYDOWN: {
                    const auto key = event.key.keysym.scancode;
                    gsl::at(key_states_, std::size_t(key)) = 1;

                    const auto window_id = event.key.windowID;

                    KeyPressEvent key_event{ KeyCode(key) };
                    VT_LOG_DEBUG("Key pressed: {}", key);

                    auto &key_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(KeyPressEvent::event_type()));
                    for (auto &client : key_event_clients)
                    {
                        if (client.callback != nullptr &&
                            (client.id == -1 ||
                                (client.id >= 0 && std::uint32_t(client.id) == window_id)))
                        {
                            if (!client.callback(client.id, &key_event, client.callback_data))
                                break;
                        }
                    }

                    break;
                }
                }
            }
        }

        const auto elapsed = float(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::high_resolution_clock::now() - start)
                                       .count()) /
                             1000.F;
        start = std::chrono::high_resolution_clock::now();

        RenderEvent render_event{ elapsed, key_states_, mouse_button_states_ };
        auto &render_clients = gsl::at(clients_, static_cast<std::size_t>(EventType::Render));
        for (auto &client : render_clients)
        {
            if (!client.callback(-1, &render_event, client.callback_data))
                break;
        }

        bgfx::frame();
    }
}

VOOT_END_NAMESPACE
