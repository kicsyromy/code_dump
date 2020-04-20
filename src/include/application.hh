#pragma once

#include <cstdint>
#include <vector>

class Application
{
public:
    static Application *application() noexcept;

public:
    Application() noexcept;
    ~Application() noexcept;

public:
    void run() noexcept;

private:
    void register_window(class Window *window) noexcept;

private:
    std::vector<class Window *> registered_windows_{};

private:
    friend class Window;
};
