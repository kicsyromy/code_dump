#include "application.hh"
#include "window.hh"

#include <GL/glew.h>

#include <spdlog/spdlog.h>

Window::Window(int width, int height) noexcept
  : mouse_last_{ 0, 0 }
  , mouse_delta_{ 0, 0 }
  , handle_{ SDL_CreateWindow(APPLICATION_NAME,
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
                      "creating a window");
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

void Window::on_window_event(SDL_WindowEvent event) noexcept
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

void Window::on_keyboard_event(SDL_KeyboardEvent event) noexcept
{
    if (event.keysym.sym == SDLK_ESCAPE && event.state == SDL_PRESSED)
    {
        const auto mouse_grabbed = SDL_GetWindowGrab(handle_.get());
        SDL_ShowCursor(mouse_grabbed);
        SDL_SetWindowGrab(handle_.get(), SDL_bool(!mouse_grabbed));
    }

    if (event.state)
        key_pressed.emit(int(event.keysym.scancode));
    else
        key_released.emit(int(event.keysym.scancode));
}

void Window::on_mouse_move_event(SDL_MouseMotionEvent event) noexcept
{
    mouse_delta_ = { GLfloat(event.xrel), GLfloat(-event.yrel) };
    mouse_last_ = { GLfloat(event.x), GLfloat(event.y) };

    if (SDL_GetWindowGrab(handle_.get())) mouse_moved.emit(mouse_delta_.x, mouse_delta_.y);
}

void Window::on_mouse_button_event(SDL_MouseButtonEvent event) noexcept
{
    const auto mouse_grabbed = SDL_GetWindowGrab(handle_.get());
    if (event.button == SDL_BUTTON_LEFT && event.state == SDL_PRESSED && !mouse_grabbed)
    {
        SDL_ShowCursor(mouse_grabbed);
        SDL_SetWindowGrab(handle_.get(), SDL_bool(!mouse_grabbed));
        ignore_next_mouse_event_ = true;

        return;
    }

    if (!ignore_next_mouse_event_)
    { mouse_button_event.emit(GLfloat(event.x), GLfloat(event.y), event.button, event.state); }

    ignore_next_mouse_event_ = false;
}

void Window::on_mouse_wheel_event(SDL_MouseWheelEvent event) noexcept { static_cast<void>(event); }
