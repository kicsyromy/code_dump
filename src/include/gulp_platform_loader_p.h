#ifndef GULP_PLATFORM_LOADER_P_H
#define GULP_PLATFORM_LOADER_P_H

namespace gulp
{
    class Platform;

    class PlatformLoader
    {
    public:
        PlatformLoader(int argc, char *argv[]);
        ~PlatformLoader();

    public:
        inline Platform &get() { return *loadedPlatform_; }

    private:
        void *handle_;
        Platform *loadedPlatform_;
    };
}

#endif // GULP_PLATFORM_LOADER_P_H
