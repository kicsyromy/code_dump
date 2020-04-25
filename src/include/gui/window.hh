#pragma once

#include "signal.hh"

#include <SDL.h>
#include <GL/glew.h>

#include <array>
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
    Signal<GLfloat, GLfloat> mouse_moved;
    Signal<GLfloat, GLfloat, int, bool> mouse_button_event;
    Signal<int> key_pressed;
    Signal<int> key_released;

public:
    std::pair<int, int> viewport_size() const noexcept;

    template<typename Function> void set_draw_function(Function &&f) noexcept
    {
        static_assert(std::is_invocable_r_v<void, Function, float>);
        draw_ = f;
    }

private:
    void on_window_event(SDL_WindowEvent event) noexcept;
    void on_keyboard_event(SDL_KeyboardEvent event) noexcept;
    void on_mouse_move_event(SDL_MouseMotionEvent event) noexcept;
    void on_mouse_button_event(SDL_MouseButtonEvent event) noexcept;
    void on_mouse_wheel_event(SDL_MouseWheelEvent event) noexcept;

private:
    std::function<void(float)> draw_{ nullptr };

private:
    struct
    {
        GLfloat x, y;
    } mouse_last_, mouse_delta_;
    bool ignore_next_mouse_event_{ false };

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> handle_;
    std::unique_ptr<void, decltype(&SDL_GL_DeleteContext)> gl_context_;

private:
    friend class Application;
};
