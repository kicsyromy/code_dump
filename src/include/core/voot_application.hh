#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"
#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"

#include <gsl/gsl>

#include <array>
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
    void register_event_handler(EventType,
        EventCallback,
        void *user_data,
        int priority = 0) noexcept;
    void quit() noexcept
    {
        quit_ = true;
    }
    void exec();

private:
    bool quit_{ false };
    std::array<std::uint8_t, KEY_CODE_COUNT> key_states_{ 0 };
    std::array<std::uint8_t, MOUSE_BUTTON_COUNT> mouse_button_states_{ 0 };
    std::array<std::vector<std::tuple<int, EventCallback, void *>>, EVENT_TYPE_COUNT> clients_;
};

VOOT_END_NAMESPACE
