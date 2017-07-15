#include "gulp_gtk_platform.h"

#include <gtk/gtk.h>

#include "gulp_gtk_window.h"

extern "C" gulp::GtkPlatform *loadPlatform(int argc, char *argv[])
{
    return new gulp::GtkPlatform(argc, argv);
}

using namespace gulp;

GtkPlatform::GtkPlatform(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
}

int GtkPlatform::startMainLoop()
{
    gtk_main();
}

Platform::TypeInitializers GtkPlatform::initializers()
{
    return {
        &gulp::GtkWindow::create
    };
}
