#ifndef GULP_APPLICATION_P_H
#define GULP_APPLICATION_P_H

namespace gulp
{
    class Platform;

    class ApplicationPrivate
    {
    public:
        ApplicationPrivate(int argc, char *argv[]);
        ~ApplicationPrivate() = default;

    public:
        int run();

    private:
        Platform &platform_;
    };
}

#endif // GULP_APPLICATION_P_H
