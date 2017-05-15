#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <cairo.h>

#include "event.h"

namespace ui
{
    class Window
    {
        using mouse_event_callback_t = void (*)(const MouseEvent &, void *userData);
        using keypress_event_callback_t = void (*)(const KeyEvent &, void *userData);
        using paint_callback_t = void (*)(cairo_t *, std::size_t width, std::size_t height, void *userData);

    public:
        Window(const std::string &title, std::size_t width, std::size_t height);
        ~Window();

    public:
        void show();
        void hide();

    public:
        void setMouseHandler(mouse_event_callback_t &&f, void *userData = nullptr);
        void setKeyPressHandler(keypress_event_callback_t &&f, void *userData = nullptr);
        void setPaintHandler(paint_callback_t &&f, void *userData = nullptr);
        void repaint();

    private:
        void mouseEvent(const MouseEvent &e);
        void keyPressEvent(const KeyEvent &e);
        void drawEvent(std::size_t width, std::size_t height);
        void closeEvent();

    private:
        void *handle_;
        cairo_surface_t *surface_;
        cairo_t *context_;
        struct {
            mouse_event_callback_t function = nullptr;
            void *userData = nullptr;
        } mouseEventCallback_;
        struct {
            keypress_event_callback_t function = nullptr;
            void *userData = nullptr;
        } keyPressEventCallback_;
        struct {
            paint_callback_t function = nullptr;
            void *userData = nullptr;
        } paintCallback_;

    private:
        friend class Application;
    };
}

#endif // WINDOW_H
