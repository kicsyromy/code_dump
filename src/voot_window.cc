#include "voot_global.hh"
#include "core/voot_logger.hh"
#include "gui/voot_window.hh"

#include <SDL2.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <nanovg/nanovg.h>

namespace
{
    /* FIXME: Clarify is this needs to be thread safe or not */
    /* FIXME: Clarify is this is the right way to keep track is view indices */
    std::uint16_t view_id_last = 1;

    constexpr auto w_width{ 800 };
    constexpr auto w_height{ 800 };

    void update_window_surface(std::uint16_t view_id,
        std::uint16_t framebuffer_handle,
        std::uint16_t width,
        std::uint16_t height) noexcept
    {
        bgfx::setViewFrameBuffer(view_id, bgfx::FrameBufferHandle{ framebuffer_handle });
        bgfx::setViewRect(view_id, 0, 0, width, height);
        bgfx::setViewClear(view_id,
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
            voot::utility::rgba(0, 0, 0),
            1.0F,
            0);
        bgfx::touch(view_id);
    }

} // namespace

VOOT_BEGIN_NAMESPACE

Window::Window(std::string_view title) noexcept
  : width_{ w_width }
  , height_{ w_height }
  , window_handle_{ SDL_CreateWindow(title.data(),
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED,
                        int(width_),
                        int(height_),
                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
      &SDL_DestroyWindow }
  , view_id_{ view_id_last++ }
  , framebuffer_handle_{ bgfx::createFrameBuffer(native_window_handle(),
        std::uint16_t(width_),
        std::uint16_t(height_))
                             .idx }
  , drawing_context_{ nvgCreate(true, view_id_), &nvgDelete }
{
    if (window_handle_ == nullptr)
    {
        VT_LOG_FATAL("Failed to create window: {}", SDL_GetError());
    }

    update_window_surface(view_id_,
        framebuffer_handle_,
        std::uint16_t(width_),
        std::uint16_t(height_));

    auto *app = VT_APPLICATION();
    assert(app != nullptr);

    app->register_event_handler(EventType::KeyPressed,
        VT_EVENT_HANDLER(&Window::on_key_press_event),
        this);
    app->register_event_handler(EventType::KeyReleased,
        VT_EVENT_HANDLER(&Window::on_key_release_event),
        this);
    app->register_event_handler(EventType::MouseMoved,
        VT_EVENT_HANDLER(&Window::on_mouse_move_event),
        this);
    app->register_event_handler(EventType::MouseButtonPressed,
        VT_EVENT_HANDLER(&Window::on_mouse_button_press_event),
        this);
    app->register_event_handler(EventType::MouseButtonReleased,
        VT_EVENT_HANDLER(&Window::on_mouse_button_release_event),
        this);
    app->register_event_handler(EventType::WindowResized,
        VT_EVENT_HANDLER(&Window::on_window_resized_event),
        this);
    app->register_event_handler(EventType::Render,
        VT_EVENT_HANDLER(&Window::on_render_event),
        this);
}

Window::~Window() noexcept
{
    bgfx::destroy(bgfx::FrameBufferHandle{ framebuffer_handle_ });
    framebuffer_handle_ = bgfx::kInvalidHandle;
    bgfx::setViewFrameBuffer(view_id_, bgfx::FrameBufferHandle{ bgfx::kInvalidHandle });

    bgfx::frame();
    bgfx::frame();
}

std::pair<std::size_t, std::size_t> voot::Window::viewport_size() const noexcept
{
    return { 0, 0 };
}

void *Window::native_window_handle() const noexcept
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window_handle_.get(), &wmi))
    {
        VT_LOG_ERROR("Failed to get native window handle: {}", SDL_GetError());

        return nullptr;
    }

#ifdef _WIN32
    return reinterpret_cast<void *>(wmi.info.win.window);
#else
    if (!VT_WAYLAND_DISPLAY.empty())
        return wmi.info.wl.surface;

    return reinterpret_cast<void *>(wmi.info.x11.window);
#endif
}

bool Window::on_key_press_event(int window_id, KeyPressEvent *event, Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);
    static_cast<void>(self);

    VT_LOG_DEBUG("[window] key press");

    return true;
}

bool Window::on_key_release_event(int window_id, KeyReleaseEvent *event, Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);
    static_cast<void>(self);

    VT_LOG_DEBUG("[window] key release");

    return true;
}

bool Window::on_mouse_move_event(int window_id, MouseMoveEvent *event, Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);
    static_cast<void>(self);

    VT_LOG_DEBUG("[window] mouse move");

    return true;
}

bool Window::on_mouse_button_press_event(int window_id,
    MouseButtonPressEvent *event,
    Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);
    static_cast<void>(self);

    VT_LOG_DEBUG("[window] mouse button press");

    return true;
}

bool Window::on_mouse_button_release_event(int window_id,
    MouseButtonReleaseEvent *event,
    Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);
    static_cast<void>(self);

    VT_LOG_DEBUG("[window] mouse button release");

    return true;
}

bool Window::on_window_resized_event(int window_id, WindowResizeEvent *event, Window *self) noexcept
{
    static_cast<void>(window_id);

    auto [new_width, new_height] = event->size();
    self->width_ = new_width;
    self->height_ = new_height;

    bgfx::destroy(bgfx::FrameBufferHandle{ self->framebuffer_handle_ });
    self->framebuffer_handle_ = bgfx::createFrameBuffer(self->native_window_handle(),
        std::uint16_t(new_width),
        std::uint16_t(new_height))
                                    .idx;
    update_window_surface(self->view_id_,
        self->framebuffer_handle_,
        std::uint16_t(new_width),
        std::uint16_t(new_height));

    return true;
}

bool Window::on_window_closed_event(int window_id, WindowCloseEvent *event, Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);
    static_cast<void>(self);

    VT_LOG_DEBUG("[window] window closed");

    return true;
}

bool Window::on_render_event(int window_id, RenderEvent *event, Window *self) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    bgfx::touch(self->view_id_);

    auto vg = self->drawing_context_.get();
    nvgBeginFrame(vg, float(self->width_), float(self->height_), 1.0F);

    nvgBeginPath(vg);
    nvgRect(vg, float(self->width_ - 140), 100, 120, 30);
    nvgFillColor(vg, nvgRGBA(0, 0, 255, 255));
    nvgFill(vg);

    nvgEndFrame(vg);

    return true;
}

VOOT_END_NAMESPACE
