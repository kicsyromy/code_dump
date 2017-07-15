#include "gulp_gtk_window.h"

gulp::PlatformWindow *gulp::GtkWindow::create()
{
    return new GtkWindow();
}

gulp::GtkWindow::GtkWindow()
  : handle_(gtk_window_new(GTK_WINDOW_TOPLEVEL))
{
}

gulp::GtkWindow::~GtkWindow()
{
    gtk_widget_hide(handle_);
    gtk_widget_destroy(handle_);
}

void gulp::GtkWindow::show()
{
    gtk_widget_show_all(handle_);
}
