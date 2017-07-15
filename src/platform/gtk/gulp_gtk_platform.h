#ifndef GULP_GTK_PLATFORM_H
#define GULP_GTK_PLATFORM_H

#include <gulp_platform_p.h>

namespace gulp
{
    class GtkPlatform: public Platform
    {
    public:
        GtkPlatform(int argc, char *argv[]);
        ~GtkPlatform() {}

    public:
        TypeInitializers initializers() override;

    public:
        int startMainLoop() override;
    };
}

#endif // GULP_GTK_PLATFORM_H
