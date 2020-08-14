#include "core/voot_application.hh"

#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"
#include "events/voot_render_event.hh"
#include "events/voot_window_events.hh"

#include <SDL2.h>
#include <gpu/GrDirectContext.h>
#include <gpu/gl/GrGLInterface.h>

#include <unordered_set>
#include <variant>

namespace
{
    voot::Application *g_app_instance = nullptr;

    const std::uint32_t USER_EVENT_TYPE{ [] {
        return SDL_RegisterEvents(1);
    }() };

    struct EventWrapper
    {
        EventWrapper(voot::Event *e, void (*deleter)(voot::Event *)) noexcept
          : event{ std::unique_ptr<voot::Event, decltype(deleter)>{ e, deleter } }
        {}

        EventWrapper(const std::shared_ptr<voot::Event> &e) noexcept
          : event{ e }
        {}

        std::variant<std::unique_ptr<voot::Event>, std::shared_ptr<voot::Event>> event;
    };

    voot::Event *get_window_event(const SDL_WindowEvent &event_data,
        voot::WindowEventVariant &window_event_var)
    {
        using namespace voot;

        Event *window_event{ nullptr };
        switch ((event_data).event)
        {
        default:
            break;
        case SDL_WINDOWEVENT_SHOWN:
        case SDL_WINDOWEVENT_EXPOSED:
            window_event_var = voot::WindowShowEvent{};
            (window_event) = &(std::get<voot::WindowShowEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            window_event_var = voot::WindowHideEvent{};
            (window_event) = &(std::get<voot::WindowHideEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_MOVED:
            window_event_var = voot::WindowMoveEvent{ (event_data).data1, (event_data).data2 };
            (window_event) = &(std::get<voot::WindowMoveEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            window_event_var =
                voot::WindowResizeEvent{ static_cast<std::uint16_t>((event_data).data1),
                    static_cast<std::uint16_t>((event_data).data2) };
            (window_event) = &(std::get<voot::WindowResizeEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
        case SDL_WINDOWEVENT_MAXIMIZED:
        case SDL_WINDOWEVENT_RESTORED:
        case SDL_WINDOWEVENT_ENTER:
        case SDL_WINDOWEVENT_LEAVE:
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            window_event_var = voot::WindowGainFocusEvent{};
            (window_event) = &(std::get<voot::WindowGainFocusEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            window_event_var = voot::WindowLooseFocusEvent{};
            (window_event) = &(std::get<voot::WindowLooseFocusEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_CLOSE:
            window_event_var = voot::WindowCloseEvent{};
            (window_event) = &(std::get<voot::WindowCloseEvent>(window_event_var));
            break;
        case SDL_WINDOWEVENT_TAKE_FOCUS:
        case SDL_WINDOWEVENT_HIT_TEST:
            break;
        }

        return window_event;
    }
} // namespace

VT_BEGIN_NAMESPACE

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

    g_app_instance = this;
}

Application::~Application() noexcept
{
    g_app_instance = nullptr;
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
                        if (!client.callback()(client.id(), e, client.callback_data()))
                            break;
                    }

                    delete wrapped_event;

                    continue;
                }

                switch (event.type)
                {
                case SDL_WINDOWEVENT: {
                    voot::WindowEventVariant window_event_var;
                    auto *window_event = get_window_event(event.window, window_event_var);

                    if (window_event != nullptr)
                    {
                        const auto window_id = event.window.windowID;
                        VT_LOG_DEBUG("New window event: {}", window_event->event_name());

                        auto &window_clients =
                            gsl::at(clients_, gsl::index(window_event->event_type()));
                        for (auto &client : window_clients)
                        {
                            if (client.callback() != nullptr &&
                                (client.id() == -1 ||
                                    (client.id() >= 0 && std::uint32_t(client.id()) == window_id)))
                            {
                                if (!client.callback()(client.id(),
                                        window_event,
                                        client.callback_data()))
                                    break;
                            }
                        }

                        if (window_event->event_type() == EventType::WindowShown)
                        {
                            if (open_windows.count(window_id) == 0)
                            {
                                open_windows.insert(window_id);
#ifdef __WIN32__
                                /* For some reason the Windows Display Manager forgets to update */
                                /* the window size when first opening a new window, and the      */
                                /* content is not rendered at the proper coordinates so force a  */
                                /* resize event                                                  */

                                int window_width;
                                int window_height;
                                SDL_GetWindowSize(SDL_GetWindowFromID(window_id),
                                    &window_width,
                                    &window_height);

                                WindowResizeEvent fake_resize_event{ std::uint16_t(window_width),
                                    std::uint16_t(window_height) };

                                auto &window_clients_fake_resize =
                                    gsl::at(clients_, gsl::index(EventType::WindowResized));
                                for (auto &client : window_clients_fake_resize)
                                {
                                    if (client.callback() != nullptr &&
                                        (client.id() >= 0 &&
                                            std::uint32_t(client.id()) == window_id))
                                    {
                                        if (!client.callback()(client.id(),
                                                &fake_resize_event,
                                                client.callback_data()))
                                            break;
                                    }
                                }
#endif
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
                        gsl::at(clients_, gsl::index(MouseMoveEvent::event_type()));
                    for (auto &client : mouse_event_clients)
                    {
                        if (client.callback() != nullptr &&
                            (client.id() == -1 ||
                                (client.id() >= 0 && std::uint32_t(client.id()) == window_id)))
                        {
                            if (!client.callback()(client.id(),
                                    &mouse_move_event,
                                    client.callback_data()))
                                break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    const auto mouse_button = static_cast<MouseButton>(event.button.button - 1);
                    gsl::at(mouse_button_states_, gsl::index(mouse_button)) = 1;

                    const auto window_id = event.button.windowID;

                    MouseButtonPressEvent mouse_press_event{ event.button.x,
                        event.button.y,
                        mouse_button };
                    VT_LOG_DEBUG("Mouse button pressed: B: {}, X: {}, Y: {}",
                        static_cast<int>(mouse_press_event.button()),
                        mouse_press_event.coordinates().first,
                        mouse_press_event.coordinates().second);

                    auto &mouse_event_clients =
                        gsl::at(clients_, gsl::index(MouseButtonPressEvent::event_type()));
                    for (auto &client : mouse_event_clients)
                    {
                        if (client.callback() != nullptr &&
                            (client.id() == -1 ||
                                (client.id() >= 0 && std::uint32_t(client.id()) == window_id)))
                        {
                            if (!client.callback()(client.id(),
                                    &mouse_press_event,
                                    client.callback_data()))
                                break;
                        }
                    }

                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    const auto mouse_button = static_cast<MouseButton>(event.button.button - 1);
                    gsl::at(mouse_button_states_, gsl::index(mouse_button)) = 0;

                    const auto window_id = event.button.windowID;

                    MouseButtonReleaseEvent mouse_release_event{ event.button.x,
                        event.button.y,
                        mouse_button };
                    VT_LOG_DEBUG("Mouse button released: B: {}, X: {}, Y: {}",
                        static_cast<int>(mouse_release_event.button()),
                        mouse_release_event.coordinates().first,
                        mouse_release_event.coordinates().second);

                    auto &mouse_event_clients =
                        gsl::at(clients_, gsl::index(MouseButtonReleaseEvent::event_type()));
                    for (auto &client : mouse_event_clients)
                    {
                        if (client.callback() != nullptr &&
                            (client.id() == -1 ||
                                (client.id() >= 0 && std::uint32_t(client.id()) == window_id)))
                        {
                            if (!client.callback()(client.id(),
                                    &mouse_release_event,
                                    client.callback_data()))
                                break;
                        }
                    }

                    break;
                }
                case SDL_KEYUP: {
                    const auto key = event.key.keysym.scancode;
                    gsl::at(key_states_, gsl::index(key)) = 0;

                    const auto window_id = event.key.windowID;

                    KeyReleaseEvent key_event{ KeyCode(key) };
                    VT_LOG_DEBUG("Key released: {}", key);

                    auto &key_event_clients =
                        gsl::at(clients_, gsl::index(KeyReleaseEvent::event_type()));
                    for (auto &client : key_event_clients)
                    {
                        if (client.callback() != nullptr &&
                            (client.id() == -1 ||
                                (client.id() >= 0 && std::uint32_t(client.id()) == window_id)))
                        {
                            if (!client.callback()(client.id(), &key_event, client.callback_data()))
                                break;
                        }
                    }

                    break;
                }
                case SDL_KEYDOWN: {
                    const auto key = event.key.keysym.scancode;
                    gsl::at(key_states_, gsl::index(key)) = 1;

                    const auto window_id = event.key.windowID;

                    KeyPressEvent key_event{ KeyCode(key) };
                    VT_LOG_DEBUG("Key pressed: {}", key);

                    auto &key_event_clients =
                        gsl::at(clients_, static_cast<std::size_t>(KeyPressEvent::event_type()));
                    for (auto &client : key_event_clients)
                    {
                        if (client.callback() != nullptr &&
                            (client.id() == -1 ||
                                (client.id() >= 0 && std::uint32_t(client.id()) == window_id)))
                        {
                            if (!client.callback()(client.id(), &key_event, client.callback_data()))
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
        auto &render_clients = gsl::at(clients_, gsl::index(EventType::Render));
        for (auto &client : render_clients)
        {
            if (!client.callback()(-1, &render_event, client.callback_data()))
                break;
        }
    }
}

void Application::post_event_owned(gsl::owner<Event *> event,
    int receiver_id,
    void (*deleter)(Event *)) noexcept
{
    VT_LOG_DEBUG("Pushing owned event to queue: {}", event->event_name());

    static_cast<void>(receiver_id);

    SDL_Event sdl_event;
    SDL_zero(sdl_event);

    sdl_event.type = USER_EVENT_TYPE;
    sdl_event.user.code = 0;
    sdl_event.user.data1 = new EventWrapper{ event, deleter };
    sdl_event.user.data2 = nullptr;

    SDL_PushEvent(&sdl_event);
}

VT_END_NAMESPACE
