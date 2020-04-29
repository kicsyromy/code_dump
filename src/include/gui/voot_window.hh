#pragma once

#include "voot_global.hh"

#include "core/voot_application.hh"

#include "gui/voot_item.hh"

#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"
#include "events/voot_render_event.hh"
#include "events/voot_window_events.hh"

#include <array>
#include <functional>
#include <memory>
#include <string_view>
#include <type_traits>

using SDL_Window = struct SDL_Window;
struct NVGcontext;

namespace bgfx
{
    struct FrameBufferHandle;
}

VOOT_BEGIN_NAMESPACE

class Window
{
public:
    Window(std::string_view title) noexcept;
    ~Window() noexcept;

public:
    std::pair<std::size_t, std::size_t> viewport_size() const noexcept;
    constexpr Item *root_item() noexcept
    {
        return &root_item_;
    }

public:
    void *native_window_handle() const noexcept;

private:
    bool on_key_press_event(int window_id, KeyPressEvent *event) noexcept;
    bool on_key_release_event(int window_id, KeyReleaseEvent *event) noexcept;
    bool on_mouse_move_event(int window_id, MouseMoveEvent *event) noexcept;
    bool on_mouse_button_press_event(int window_id, MouseButtonPressEvent *event) noexcept;
    bool on_mouse_button_release_event(int window_id, MouseButtonReleaseEvent *event) noexcept;
    //    bool on_mouse_wheel_event(int window_id, MouseWheelEvent *event) noexcept;
    bool on_window_resized_event(int window_id, WindowResizeEvent *event) noexcept;
    bool on_window_closed_event(int window_id, WindowCloseEvent *event) noexcept;
    bool on_render_event(int window_id, RenderEvent *event) noexcept;

private:
    std::uint16_t width_;
    std::uint16_t height_;

private:
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> window_handle_{ nullptr, nullptr };
    std::uint16_t view_id_;
    std::uint16_t framebuffer_handle_;
    std::unique_ptr<NVGcontext, void (*)(NVGcontext *)> drawing_context_{ nullptr, nullptr };
    Item root_item_;
};

VOOT_END_NAMESPACE
