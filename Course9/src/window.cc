#include "application.hh"
#include "window.hh"

#include <GL/glew.h>

#include <spdlog/spdlog.h>

Window::Window(int width, int height) noexcept
  : handle_{ SDL_CreateWindow(APPLICATION_NAME,
                 SDL_WINDOWPOS_UNDEFINED,
                 SDL_WINDOWPOS_UNDEFINED,
                 width,
                 height,
                 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
      &SDL_DestroyWindow }
  , gl_context_{ SDL_GL_CreateContext(handle_.get()), &SDL_GL_DeleteContext }
{
    if (handle_ == nullptr)
    {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    int viewport_width;
    int viewport_height;
    SDL_GL_GetDrawableSize(handle_.get(), &viewport_width, &viewport_height);
    glViewport(0, 0, viewport_width, viewport_height);

    auto *app = Application::application();
    if (app == nullptr)
    {
        spdlog::error("Missing application instance, create an instance of Application before "
                      "cerating a window");
        std::exit(EXIT_FAILURE);
    }
    app->register_window(this);
}

std::pair<int, int> Window::viewport_size() const noexcept
{
    int viewport_width;
    int viewport_height;
    SDL_GL_GetDrawableSize(handle_.get(), &viewport_width, &viewport_height);

    return { viewport_width, viewport_height };
}

Window::~Window() noexcept = default;

void Window::on_window_event(SDL_WindowEvent event)
{
    switch (event.event)
    {
    default:
        break;

    case SDL_WINDOWEVENT_RESIZED:
        int viewport_width;
        int viewport_height;
        SDL_GL_GetDrawableSize(handle_.get(), &viewport_width, &viewport_height);
        glViewport(0, 0, viewport_width, viewport_height);
        viewport_size_changed.emit(viewport_width, viewport_height);
        break;
    }
}
