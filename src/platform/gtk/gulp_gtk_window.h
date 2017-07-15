#ifndef GULP_GTK_WINDOW_H
#define GULP_GTK_WINDOW_H

#include <gulp_platform_window_p.h>

#include <gtk/gtk.h>

namespace gulp
{
    class GtkWindow: public PlatformWindow
    {
    public:
        static PlatformWindow *create();

    public:
        GtkWindow();
        ~GtkWindow();

    public:
        void show() override;

    private:
        GtkWidget *handle_;
    };
}

#endif // GULP_GTK_WINDOW_H
