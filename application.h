#ifndef APPLICATION_H
#define APPLICATION_H

#include <map>
#include <set>

#include "window.h"
#include "event.h"

namespace ui
{
    class Application
    {
    public:
        Application();
        ~Application();

    public:
        int exec();

    public:
        static Application *instance();

    private:
        void *display();

        static void registerWindow(Window &w);
        static void windowDestroyed(Window &w);

    private:
        std::map<void *, Window *> windows_;
        std::map<void *, std::set<KeyEvent>> pressedKeys_;

    private:
        friend class Window;
    };
}

#endif // APPLICATION_H
