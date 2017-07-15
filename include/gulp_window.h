#ifndef GULP_WINDOW_H
#define GULP_WINDOW_H

#include <memory>

#include <gulp_global.h>

namespace gulp
{
    class PlatformWindow;

    class GULP_API Window
    {
    public:
        Window();

    public:
        void show();

    private:
        std::unique_ptr<PlatformWindow> priv_;

    private:
        DISABLE_COPY(Window)
    };
}

#endif // GULP_WINDOW_H
