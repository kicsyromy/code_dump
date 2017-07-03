#ifndef GULP_APPLICATION_H
#define GULP_APPLICATION_H

#include <memory>

#include <gulp_global.h>

namespace gulp
{
    class ApplicationPrivate;

    class GULP_API Application
    {
    public:
        Application(int argc, char *argv[]);
        ~Application();

    public:
        static Application &instance();

    public:
        int run();

    private:
        std::unique_ptr<ApplicationPrivate> priv_;

    private:
        DISABLE_COPY(Application)
        DISABLE_MOVE(Application)
    };
}

#endif // GULP_APPLICATION_H
