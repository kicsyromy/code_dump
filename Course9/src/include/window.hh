#pragma once

#include "signal.hh"

#include <SDL2/SDL.h>

#include <functional>
#include <memory>
#include <type_traits>

class Window
{
public:
    Window(int width, int height) noexcept;
    ~Window() noexcept;

public:
    Signal<int, int> viewport_size_changed;

public:
    std::pair<int, int> viewport_size() const noexcept;

    template<typename Function> void set_draw_function(Function &&f) noexcept
    {
        static_assert(std::is_invocable_r_v<void, Function, float>);
        draw_ = f;
    }

private:
    void on_window_event(SDL_WindowEvent event);

private:
    std::function<void(float)> draw_{ nullptr };

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> handle_;
    std::unique_ptr<void, decltype(&SDL_GL_DeleteContext)> gl_context_;

private:
    friend class Application;
};
