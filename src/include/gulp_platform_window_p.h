#ifndef GULP_PLATFORM_WINDOW_P_H
#define GULP_PLATFORM_WINDOW_P_H

#include <gulp_global.h>

namespace gulp
{
    class PlatformWindow
    {
    public:
        PlatformWindow() {}
        virtual ~PlatformWindow() {}

    public:
        virtual void show() = 0;

    private:
        DISABLE_COPY(PlatformWindow)
        DISABLE_MOVE(PlatformWindow)
    };
}

#endif // GULP_PLATFORM_WINDOW_P_H
