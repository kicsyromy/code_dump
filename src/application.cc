#include "application.hh"
#include "window.hh"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <spdlog/spdlog.h>

static Application *app;

Application *Application::application() noexcept { return app; }

Application::Application() noexcept
{
    if (app != nullptr)
    {
        spdlog::error("Cannot create multiple instances of Application in one process");
        std::exit(EXIT_FAILURE);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    glewExperimental = GL_TRUE;

    app = this;
}

Application::~Application() noexcept { SDL_Quit(); }

void Application::run() noexcept
{
#define HANDLE_EVENT(window_id, callback)                                    \
    if (true)                                                                \
    {                                                                        \
        if (std::size_t(window_id) < registered_windows_.size())             \
        {                                                                    \
            const auto window = registered_windows_[std::size_t(window_id)]; \
            if (window == nullptr) continue;                                 \
            window->callback;                                                \
        }                                                                    \
    }                                                                        \
    static_cast<void>(window_id)

    auto start = std::chrono::high_resolution_clock::now();
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT) { quit = true; }
            else
            {
                switch (event.type)
                {
                case SDL_WINDOWEVENT:
                    /* HACK: If an SDL_WINDOWCLOSE_EVENT if pushed just quit the app */
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    {
                        quit = true;
                        break;
                    }
                    HANDLE_EVENT(event.window.windowID, on_window_event(event.window));
                    break;
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                    HANDLE_EVENT(event.key.windowID, on_keyboard_event(event.key));
                    break;
                case SDL_MOUSEMOTION:
                    HANDLE_EVENT(event.motion.windowID, on_mouse_move_event(event.motion));
                    break;
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                    HANDLE_EVENT(event.button.windowID, on_mouse_button_event(event.button));
                    break;
                case SDL_MOUSEWHEEL:
                    HANDLE_EVENT(event.wheel.windowID, on_mouse_wheel_event(event.wheel));
                    break;
                }
            }
        }

        const auto elapsed = float(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::high_resolution_clock::now() - start)
                                       .count()) /
                             1000.f;
        start = std::chrono::high_resolution_clock::now();

        for (auto *window : registered_windows_)
        {
            if (window == nullptr) continue;

            SDL_GL_MakeCurrent(window->handle_.get(), window->gl_context_.get());
            window->draw_(elapsed);
            SDL_GL_SwapWindow(window->handle_.get());
        }
    }
}

void Application::register_window(Window *window) noexcept
{
    if (registered_windows_.empty())
    {
        if (const auto glew_error = glewInit(); glew_error != GLEW_OK)
        {
            spdlog::error("Failed to initialize GLEW: {}", glewGetErrorString(glew_error));
            std::exit(EXIT_FAILURE);
        }
    }

    const auto id = SDL_GetWindowID(window->handle_.get());
    if (registered_windows_.size() <= id)
    { registered_windows_.resize(std::size_t(id + 1), nullptr); }
    registered_windows_[id] = window;
}
