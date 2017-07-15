#ifndef GULP_PLATFORM_H
#define GULP_PLATFORM_H

#include <tuple>

#include "gulp_global.h"

namespace gulp
{
    class PlatformWindow;

    class Platform
    {
    protected:
        struct TypeInitializers
        {
            PlatformWindow *(*window_new)();
        };
        virtual TypeInitializers initializers() = 0;

    public:
        static Platform &load(int argc, char *argv[]);
        static TypeInitializers getInitializers();

    public:
        Platform() {}
        virtual ~Platform() = default;

    public:
        virtual int startMainLoop() = 0;

    private:
        DISABLE_COPY(Platform)
        DISABLE_MOVE(Platform)
    };
}
#endif // GULP_PLATFORM_H
