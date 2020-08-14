#include "gui/voot_window.hh"
#include "core/voot_logger.hh"
#include "voot_global.hh"

#include <GL/gl.h>

#include <SDL2.h>
#include <SDL_syswm.h>
#include <core/SkCanvas.h>
#include <core/SkSurface.h>
#include <gpu/GrDirectContext.h>
#include <gpu/gl/GrGLInterface.h>

namespace
{
    constexpr auto w_width{ 800 };
    constexpr auto w_height{ 800 };
} // namespace

VT_BEGIN_NAMESPACE

Window::Window(std::string_view title) noexcept
  : width_{ w_width }
  , height_{ w_height }
  , window_handle_{ SDL_CreateWindow(title.data(),
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED,
                        int(width_),
                        int(height_),
                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
      &SDL_DestroyWindow }
  , root_item_{}
{
    if (window_handle_ == nullptr)
    {
        VT_LOG_FATAL("Failed to create window: {}", SDL_GetError());
    }

    update_window_surface(width_, height_);

    auto *app = VT_APPLICATION();
    assert(app != nullptr);

    app->register_event_handler<KeyPressEvent, &Window::on_key_press_event>(this);

    app->register_event_handler<KeyReleaseEvent, &Window::on_key_release_event>(this);

    app->register_event_handler<MouseMoveEvent, &Window::on_mouse_move_event>(this);

    app->register_event_handler<MouseButtonPressEvent, &Window::on_mouse_button_press_event>(this);

    app->register_event_handler<MouseButtonReleaseEvent, &Window::on_mouse_button_release_event>(
        this);

    app->register_event_handler<WindowResizeEvent, &Window::on_window_resized_event>(this);

    app->register_event_handler<WindowCloseEvent, &Window::on_window_closed_event>(this);

    app->register_event_handler<RenderEvent, &Window::on_render_event>(this);
}

Window::~Window() noexcept = default;

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
#elif __linux__
    return reinterpret_cast<void *>(wmi.info.x11.window);
#else
    return reinterpret_cast<void *>(wmi.info.cocoa.window);
#endif
}

bool Window::on_key_press_event(int window_id, KeyPressEvent *event) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    if (event->key() == KeyCode::Tab)
    {
        root_item_.update_focus();
    }

    return true;
}

bool Window::on_key_release_event(int window_id, KeyReleaseEvent *event) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    VT_LOG_DEBUG("[window] key release");

    return true;
}

bool Window::on_mouse_move_event(int window_id, MouseMoveEvent *event) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    VT_LOG_DEBUG("[window] mouse move");

    return true;
}

bool Window::on_mouse_button_press_event(int window_id, MouseButtonPressEvent *event) noexcept
{
    static_cast<void>(window_id);

    VT_LOG_DEBUG("[window] mouse button press B: {} X: {} Y: {}",
        event->button(),
        event->coordinates().first,
        event->coordinates().second);
    const auto [x, y] = event->coordinates();
    root_item_.handle_mouse_button_pressed(event->button(), x, y);

    return true;
}

bool Window::on_mouse_button_release_event(int window_id, MouseButtonReleaseEvent *event) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    VT_LOG_DEBUG("[window] mouse button release");
    const auto [x, y] = event->coordinates();
    root_item_.handle_mouse_button_released(event->button(), x, y);

    return true;
}

bool Window::on_window_resized_event(int window_id, WindowResizeEvent *event) noexcept
{
    static_cast<void>(window_id);

    auto [new_width, new_height] = event->size();
    update_window_surface(new_width, new_height);

    return true;
}

bool Window::on_window_closed_event(int window_id, WindowCloseEvent *event) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    VT_LOG_DEBUG("[window] window closed");

    return true;
}

bool Window::on_render_event(int window_id, RenderEvent *event) noexcept
{
    static_cast<void>(window_id);
    static_cast<void>(event);

    SDL_GL_MakeCurrent(window_handle_.get(),
        VT_APPLICATION()->graphics_context().graphics_api_context);

    glViewport(0, 0, width_, height_);
    glClearColor(1, 1, 1, 1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto *canvas = surface_->getCanvas();
    root_item_.render(canvas);
    canvas->flush();

    SDL_GL_SwapWindow(window_handle_.get());

    return true;
}

void Window::update_window_surface(std::uint16_t width, std::uint16_t height) noexcept
{
    width_ = width;
    height_ = height;

    root_item_.width = width;
    root_item_.height = height;

    auto &skia_context = VT_APPLICATION()->graphics_context().skia_context;

    auto window_format = SDL_GetWindowPixelFormat(window_handle_.get());
    int contextType;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    auto interface = GrGLMakeNativeInterface();

    GrGLint buffer;
    interface->fFunctions.fGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = GrGLuint(buffer);
    SkColorType color_type;
    if (SDL_PIXELFORMAT_RGBA8888 == window_format)
    {
        info.fFormat = GL_RGBA8;
        color_type = kRGBA_8888_SkColorType;
    }
    else
    {
        color_type = kBGRA_8888_SkColorType;
        if (SDL_GL_CONTEXT_PROFILE_ES == contextType)
        {
            info.fFormat = 0; // GL_BGRA8;
        }
        else
        {
            // We assume the internal format is RGBA8 on desktop GL
            info.fFormat = GL_RGBA8;
        }
    }

    GrBackendRenderTarget target(width, height, 0, 8, info);
    // setup SkSurface
    // To use distance field text, use commented out SkSurfaceProps instead
    // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
    //                      SkSurfaceProps::kLegacyFontHost_InitType);
    SkSurfaceProps surface_properties(SkSurfaceProps::kLegacyFontHost_InitType);
    sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(&skia_context,
        target,
        kBottomLeft_GrSurfaceOrigin,
        color_type,
        nullptr,
        &surface_properties));

    surface_ = std::unique_ptr<SkSurface>{ surface.release() };
}

VT_END_NAMESPACE
