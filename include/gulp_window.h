#ifndef GULP_WINDOW_H
#define GULP_WINDOW_H

#include <memory>

#include <gulp_global.h>

namespace gulp
{
    class WindowPrivate;

    class GULP_API Window
    {
    public:
        Window();

    private:
        std::unique_ptr<WindowPrivate> priv_;

    private:
        DISABLE_COPY(Window)
    };
}

#endif // GULP_WINDOW_H
