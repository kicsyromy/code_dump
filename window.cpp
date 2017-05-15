#include "window.h"

#include <iostream>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "application.h"

ui::Window::Window(const std::string &title, std::size_t width, std::size_t height)
    : handle_(nullptr)
    , surface_(nullptr)
    , context_(nullptr)
    , mouseEventCallback_()
    , keyPressEventCallback_()
    , paintCallback_()
{
    (void)title;

    auto app = Application::instance();
    if (app == nullptr)
    {
        std::cerr << "Windows cannot be created before an Application instance\n";
        std::terminate();
    }

    auto display = static_cast<Display *>(app->display());
    auto window = XCreateSimpleWindow(display,
                                      DefaultRootWindow(display),
                                      0, 0,
                                      width, height,
                                      0, 0, 0);
    handle_ = reinterpret_cast<void *>(window);


    auto screen = DefaultScreen(display);
    surface_ = cairo_xlib_surface_create(display,
                                         window,
                                         DefaultVisual(display, screen),
                                         width, height);
    cairo_xlib_surface_set_size(surface_, width, height);
    context_ = cairo_create(surface_);

    Application::registerWindow(*this);
}

ui::Window::~Window()
{
   cairo_destroy(context_);
   cairo_surface_destroy(surface_);
   Application::windowDestroyed(*this);
}

void ui::Window::show()
{
    XMapWindow(static_cast<Display *>(Application::instance()->display()), reinterpret_cast<XID>(handle_));
}

void ui::Window::hide()
{
    XUnmapWindow(static_cast<Display *>(Application::instance()->display()), reinterpret_cast<XID>(handle_));
}

void ui::Window::setMouseHandler(mouse_event_callback_t &&f, void *userData)
{
    mouseEventCallback_.function = f;
    mouseEventCallback_.userData = userData;
}

void ui::Window::setKeyPressHandler(keypress_event_callback_t &&f, void *userData)
{
    keyPressEventCallback_.function = f;
    keyPressEventCallback_.userData = userData;
}

void ui::Window::setPaintHandler(paint_callback_t &&f, void *userData)
{
    paintCallback_.function = f;
    paintCallback_.userData = userData;
}

void ui::Window::repaint()
{
    auto display = static_cast<Display *>(Application::instance()->display());
    auto window = reinterpret_cast<XID>(handle_);
    XClearArea(display, window, 0, 0, 1, 1, true);
    XFlush(display);
}

void ui::Window::mouseEvent(const MouseEvent &e)
{
    if (mouseEventCallback_.function != nullptr)
    {
        mouseEventCallback_.function(e, mouseEventCallback_.userData);
    }
}

void ui::Window::keyPressEvent(const KeyEvent &e)
{
    if (keyPressEventCallback_.function != nullptr)
    {
        keyPressEventCallback_.function(e, keyPressEventCallback_.userData);
    }
}

void ui::Window::drawEvent(std::size_t width, std::size_t height)
{
    cairo_xlib_surface_set_size(surface_, width, height);
    if (paintCallback_.function != nullptr)
    {
        paintCallback_.function(context_, width, height, paintCallback_.userData);
    }
    cairo_surface_flush(surface_);
}

void ui::Window::closeEvent()
{
    // For now just hide the window
    hide();
}
