#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"

#include <gsl/gsl>

#include <cstdint>
#include <memory>
#include <vector>

#define VOOT_APPLICATION() voot::Application::instance()

VOOT_BEGIN_NAMESPACE

class Application
{
public:
    static Application *instance() noexcept;

public:
    using EventCallback = bool (*)(Event *, void *);

public:
    Application() noexcept;
    ~Application() noexcept;

public:
    void post_event(gsl::owner<Event *> event) noexcept;
    void post_event(const std::shared_ptr<Event> &event) noexcept;
    void register_event_handler(EventCallback callback, void *user_data, int priority = 0) noexcept;
    void quit() noexcept
    {
        quit_ = true;
    }
    void exec();

private:
    bool quit_{ false };
    std::vector<std::tuple<int, EventCallback, void *>> clients_;
};

VOOT_END_NAMESPACE
