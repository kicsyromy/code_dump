#include "application.h"

#include <iostream>
#include <vector>

#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

namespace
{
    ui::Application *app = nullptr;

    struct XInit
    {
        XInit()
        {
            display = XOpenDisplay(nullptr);
            if (display == nullptr)
            {
                std::cerr << "Failed to open display\n";
                std::terminate();
            }
        }
        ~XInit()
        {
            XCloseDisplay(display);
        }
        Display *display = nullptr;
    };
    XInit xinit;
    Atom deleteMessage = XInternAtom(xinit.display, "WM_DELETE_WINDOW", false);
}

ui::Application::Application()
    : windows_()
{
    app = this;
}

ui::Application::~Application()
{
    app = nullptr;
}

int ui::Application::exec()
{
    XEvent e;

    auto display = xinit.display;
    std::map<XID, ui::Window *> dirtyWindows;

    for (bool running = true; running;)
    {
        if (XPending(display) || dirtyWindows.empty())
        {
            XNextEvent(display, &e);

            switch (e.type)
            {
            default:
                break;
            case ButtonPress:
            {
                MouseEvent mouseEvent {
                    e.xbutton.x_root,
                    e.xbutton.y_root,
                    e.xbutton.x,
                    e.xbutton.y,
                    e.xbutton.button == 1 ? MouseEvent::Button::Left :
                                            e.xbutton.button == 3 ? MouseEvent::Button::Right :
                                            MouseEvent::Button::Middle
                };
                auto it = windows_.find(reinterpret_cast<void *>(e.xbutton.window));
                if (it != windows_.end())
                {
                    it->second->mouseEvent(mouseEvent);
                }
                break;
            }
            case KeyPress:
            {
//                XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
                auto keys = pressedKeys_.find(reinterpret_cast<void *>(e.xkey.window));
                if (keys != pressedKeys_.end())
                {
                    auto key = XLookupKeysym(&e.xkey, 0);
                    switch (key)
                    {
                    default:
                        break;
                    case XK_Up:
                        keys->second.insert({KeyEvent::Key::Up});
                        break;
                    case XK_Down:
                        keys->second.insert({KeyEvent::Key::Down});
                        break;
                    case XK_Left:
                        keys->second.insert({KeyEvent::Key::Left});
                        break;
                    case XK_Right:
                        keys->second.insert({KeyEvent::Key::Right});
                        break;
                    }
                }
//                std::cerr << "Pressed key: " << e.xkey.keycode << '\n';
                auto it = windows_.find(reinterpret_cast<void *>(e.xkey.window));
                if (it != windows_.end())
                {
                    for (const auto &keyEvent: keys->second)
                    {
                        it->second->keyPressEvent(keyEvent);
                    }
                }
                break;
            }
            case KeyRelease:
            {
                bool realRelease = true;
                if (XEventsQueued(display, QueuedAfterReading))
                {
                    XEvent ne;
                    XPeekEvent(display, &ne);

                    realRelease = !(ne.type == KeyPress && ne.xkey.time == e.xkey.time &&
                            ne.xkey.keycode == e.xkey.keycode);
                }
                if (realRelease)
                {
                    auto keys = pressedKeys_.find(reinterpret_cast<void *>(e.xkey.window));
                    if (keys != pressedKeys_.end())
                    {
                        auto key = XLookupKeysym(&e.xkey, 0);
                        switch (key)
                        {
                        default:
                            break;
                        case XK_Up:
                            keys->second.erase({KeyEvent::Key::Up});
                            break;
                        case XK_Down:
                            keys->second.erase({KeyEvent::Key::Down});
                            break;
                        case XK_Left:
                            keys->second.erase({KeyEvent::Key::Left});
                            break;
                        case XK_Right:
                            keys->second.erase({KeyEvent::Key::Right});
                            break;
                        }
                    }
                }
                break;
            }
            case Expose:
            {
                auto w = windows_.find(reinterpret_cast<void *>(e.xexpose.window));
                auto it = dirtyWindows.find(e.xexpose.window);
                if ((w != windows_.end()) && (it == dirtyWindows.end()))
                {
                    dirtyWindows[e.xexpose.window] = w->second;
                }
                break;
            }
            case ClientMessage:
                 if (e.xclient.data.l[0] == deleteMessage)
                 {
                     auto it = windows_.find(reinterpret_cast<void *>(e.xbutton.window));
                     if (it != windows_.end())
                     {
                         it->second->closeEvent();
                         app->windows_.erase(it);
                         if (windows_.empty())
                         {
                             running = false;
                         }
                     }
                 }
                break;
            }
        }
        else
        {
            XWindowAttributes attributes;
            for (auto window: dirtyWindows)
            {
                XGetWindowAttributes(display, window.first, &attributes);
                // Just repaint the whole thing once
                window.second->drawEvent(attributes.width, attributes.height);
            }
            dirtyWindows.clear();
        }
    }

    return 0;
}

void *ui::Application::display()
{
    return xinit.display;
}

ui::Application *ui::Application::instance()
{
    return app;
}

void ui::Application::registerWindow(ui::Window &w)
{
    app->windows_[w.handle_] = &w;
    app->pressedKeys_[w.handle_] = {};

    auto display = static_cast<Display *>(app->display());
    auto window = reinterpret_cast<XID>(w.handle_);
    XSetWMProtocols(display, window, &deleteMessage, 1);
    XSelectInput(display, window,
                 ButtonPressMask | KeyPressMask | KeyReleaseMask | ExposureMask | StructureNotifyMask);
}

void ui::Application::windowDestroyed(ui::Window &w)
{
    auto it = app->windows_.find(w.handle_);
    if (it != app->windows_.end())
    {
        app->windows_.erase(it);
    }
}
