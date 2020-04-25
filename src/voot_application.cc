#include "core/voot_application.hh"
#include "core/voot_logger.hh"

#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"
#include "events/voot_render_event.hh"
#include "events/voot_window_events.hh"

#include <SDL2.h>

#include <variant>

namespace
{
    voot::Application *g_app_instance = nullptr;
    const std::uint32_t USER_EVENT_TYPE{ [] {
        return SDL_RegisterEvents(1);
    }() };

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
        case SDL_WINDOWEVENT_HIDDEN:                                                             \
        case SDL_WINDOWEVENT_EXPOSED:                                                            \
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
    } while (true)
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

    g_app_instance = this;
}

Application::~Application() noexcept
{
    g_app_instance = nullptr;
    SDL_Quit();
}

void Application::post_event(gsl::owner<Event *> event) noexcept
{
    VT_LOG_DEBUG("Pushing owned event to queue: {}", event->event_name());

    SDL_Event sdl_event;
    SDL_zero(sdl_event);

    sdl_event.type = USER_EVENT_TYPE;
    sdl_event.user.code = 0;
    sdl_event.user.data1 = new EventWrapper{ event };
    sdl_event.user.data2 = nullptr;

    SDL_PushEvent(&sdl_event);
}

void Application::post_event(const std::shared_ptr<Event> &event) noexcept
{
    VT_LOG_DEBUG("Pushing event to queue: {}", event->event_name());

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
    int priority) noexcept
{
    auto &event_clients = gsl::at(clients_, std::size_t(type));

    event_clients.emplace_back(priority, callback, user_data);
    std::sort(event_clients.begin(),
        event_clients.end(),
        [](const auto &c1, const auto &c2) noexcept -> bool {
            return std::get<0>(c1) < std::get<0>(c2);
        });
}

void Application::exec()
{
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
                    for (auto &[priority, callback, user_data] :
                        clients_[static_cast<std::size_t>(EventType::User)])
                    {
                        if (!callback(e, user_data))
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

                    VT_LOG_DEBUG("New window event: {}", window_event->event_name());

                    if (window_event != nullptr)
                    {
                        auto &window_clients =
                            gsl::at(clients_, static_cast<std::size_t>(window_event->event_type()));
                        for (auto &[priority, callback, user_data] : window_clients)
                        {
                            if (!callback(window_event, user_data))
                                break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    MouseMoveEvent mouse_move_event{ event.motion.x, event.motion.y };
                    VT_LOG_DEBUG("Mouse moved: X: {}, Y: {}",
                        mouse_move_event.coordinates().first,
                        mouse_move_event.coordinates().second);

                    auto &mouse_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(MouseMoveEvent::event_type()));
                    for (auto &[priority, callback, user_data] : mouse_event_clients)
                    {
                        if (!callback(&mouse_move_event, user_data))
                            break;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    const auto mouse_button = static_cast<MouseButton>(event.button.button - 1);
                    gsl::at(mouse_button_states_, std::size_t(mouse_button)) = 1;

                    MouseButtonPressEvent mouse_press_event{ event.button.x,
                        event.button.y,
                        mouse_button };
                    VT_LOG_DEBUG("Mouse button pressed: B: {}, X: {}, Y: {}",
                        static_cast<int>(mouse_press_event.button()),
                        mouse_press_event.coordinates().first,
                        mouse_press_event.coordinates().second);

                    auto &mouse_event_clients = gsl::at(clients_,
                        static_cast<std::size_t>(MouseButtonPressEvent::event_type()));
                    for (auto &[priority, callback, user_data] : mouse_event_clients)
                    {
                        if (!callback(&mouse_press_event, user_data))
                            break;
                    }

                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    const auto mouse_button = static_cast<MouseButton>(event.button.button - 1);
                    gsl::at(mouse_button_states_, std::size_t(mouse_button)) = 0;

                    MouseButtonReleaseEvent mouse_release_event{ event.button.x,
                        event.button.y,
                        mouse_button };
                    VT_LOG_DEBUG("Mouse button released: B: {}, X: {}, Y: {}",
                        static_cast<int>(mouse_release_event.button()),
                        mouse_release_event.coordinates().first,
                        mouse_release_event.coordinates().second);

                    auto &mouse_event_clients = gsl::at(clients_,
                        static_cast<std::size_t>(MouseButtonReleaseEvent::event_type()));
                    for (auto &[priority, callback, user_data] : mouse_event_clients)
                    {
                        if (!callback(&mouse_release_event, user_data))
                            break;
                    }

                    break;
                }
                case SDL_KEYUP: {
                    const auto key = event.key.keysym.scancode;
                    gsl::at(key_states_, std::size_t(key)) = 0;

                    KeyReleaseEvent key_event{ KeyCode(key) };
                    VT_LOG_DEBUG("Key released: {}", key);

                    auto &mouse_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(KeyReleaseEvent::event_type()));
                    for (auto &[priority, callback, user_data] : mouse_event_clients)
                    {
                        if (!callback(&key_event, user_data))
                            break;
                    }

                    break;
                }
                case SDL_KEYDOWN: {
                    const auto key = event.key.keysym.scancode;
                    gsl::at(key_states_, std::size_t(key)) = 1;

                    KeyPressEvent key_event{ KeyCode(key) };
                    VT_LOG_DEBUG("Key pressed: {}", key);

                    auto &mouse_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(KeyPressEvent::event_type()));
                    for (auto &[priority, callback, user_data] : mouse_event_clients)
                    {
                        if (!callback(&key_event, user_data))
                            break;
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
        VT_LOG_DEBUG("{} seconds elapsed since the last frame", elapsed);
        start = std::chrono::high_resolution_clock::now();

        RenderEvent render_event{ elapsed, key_states_, mouse_button_states_ };
        auto &render_clients = gsl::at(clients_, static_cast<std::size_t>(EventType::Render));
        for (auto &[priority, callback, user_data] : render_clients)
        {
            if (!callback(&render_event, user_data))
                break;
        }
        break;
    }
}

VOOT_END_NAMESPACE
