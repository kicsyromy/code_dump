#include "core/voot_application.hh"
#include "core/voot_logger.hh"

#include <SDL2.h>

#include <variant>

namespace
{
    voot::Application *g_app_instance = nullptr;
    const std::uint32_t INTERNAL_EVENT_TYPE{ [] {
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
}

void Application::post_event(gsl::owner<Event *> event) noexcept
{
    VT_LOG_DEBUG("Pushing owned event to queue: {}", event->event_name());

    SDL_Event sdl_event;
    SDL_zero(sdl_event);

    sdl_event.type = INTERNAL_EVENT_TYPE;
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

    sdl_event.type = INTERNAL_EVENT_TYPE;
    sdl_event.user.code = 0;
    sdl_event.user.data1 = new EventWrapper{ event };
    sdl_event.user.data2 = nullptr;

    SDL_PushEvent(&sdl_event);
}

void Application::register_event_handler(EventCallback callback,
    void *user_data,
    int priority) noexcept
{
    clients_.emplace_back(priority, callback, user_data);
    std::sort(clients_.begin(),
        clients_.end(),
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
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit_ = true;
            }
            else
            {
                if (event.type == INTERNAL_EVENT_TYPE)
                {
                    auto *wrapped_event = static_cast<EventWrapper *>(event.user.data1);
                    const auto visitor = [](auto &e) -> Event * {
                        return e.get();
                    };
                    auto *e = std::visit(visitor, wrapped_event->event);
                    VT_LOG_DEBUG("Posted event: {}", e->event_name());
                    for (auto &[priority, callback, user_data] : clients_)
                    {
                        if (!callback(e, user_data))
                            break;
                    }
                    delete wrapped_event;

                    continue;
                }

                // switch (event.type)
                // {
                // }
            }
        }

        const auto elapsed = float(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::high_resolution_clock::now() - start)
                                       .count()) /
                             1000.F;
        start = std::chrono::high_resolution_clock::now();
        static_cast<void>(elapsed);
    }
}

VOOT_END_NAMESPACE
