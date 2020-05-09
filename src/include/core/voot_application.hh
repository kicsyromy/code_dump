#pragma once

#include "voot_global.hh"
#include "core/voot_logger.hh"
#include "events/voot_event.hh"
#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"

#include <gsl/gsl>

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#define VT_APPLICATION() voot::Application::instance()

using SDL_Window = struct SDL_Window;

VT_BEGIN_NAMESPACE

class VOOT_API Application
{
public:
    static Application *instance() noexcept;

public:
    using EventCallback = bool (*)(int, Event *, void *);
    template<typename Class, typename EvType>
    using EventCallbackMethod = bool (Class::*)(int, EvType *);

public:
    Application() noexcept;
    ~Application() noexcept;

public:
    template<typename EvType>
    void post_event(gsl::owner<EvType *> event, int receiver_id = -1) noexcept
    {
        post_event_owned(event, receiver_id, [](Event *e) {
            delete static_cast<EvType *>(e);
        });
    }

    void post_event(const std::shared_ptr<Event> &event, int receiver_id = -1) noexcept;

    template<typename EvType,
        auto callback,
        typename Client = typename utility::GetClassType<decltype(callback)>::Type>
    void register_event_handler(Client *instance, int user_id = -1, int priority = 0) noexcept
    {
        static_assert(
            std::is_convertible_v<decltype(callback), EventCallbackMethod<Client, EvType>>);

        auto &event_clients = gsl::at(clients_, gsl::index(EvType::event_type()));

        event_clients.emplace_back(
            user_id,
            priority,
            [](int id, Event *e, void *c) -> bool {
                auto *client = static_cast<Client *>(c);
                auto *event = static_cast<EvType *>(e);
                return (client->*callback)(id, event);
            },
            instance);

        std::sort(event_clients.begin(),
            event_clients.end(),
            [](const EventClient &c1, const EventClient &c2) noexcept -> bool {
                return c1.priority() < c2.priority();
            });
    }

    void quit() noexcept
    {
        quit_ = true;
    }
    void exec();

private:
    void post_event_owned(gsl::owner<Event *> event,
        int receiver_id,
        void (*deleter)(Event *)) noexcept;

private:
    class EventClient
    {
    public:
        constexpr EventClient(int id,
            int priority,
            EventCallback event_cb,
            void *callback_data) noexcept
          : id_{ id }
          , priority_{ priority }
          , callback_{ event_cb }
          , callback_data_{ callback_data }
        {}

    public:
        constexpr int id() const noexcept
        {
            return id_;
        }

        constexpr int priority() const noexcept
        {
            return priority_;
        }

        constexpr EventCallback callback() const noexcept
        {
            return callback_;
        }

        constexpr void *callback_data() const noexcept
        {
            return callback_data_;
        }

    private:
        int id_;
        int priority_;
        EventCallback callback_;
        void *callback_data_;
    };

private:
    bool quit_{ false };
    std::array<std::uint8_t, KEY_CODE_COUNT> key_states_{ 0 };
    std::array<std::uint8_t, MOUSE_BUTTON_COUNT> mouse_button_states_{ 0 };
    std::array<std::vector<EventClient>, EVENT_TYPE_COUNT> clients_;

private:
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> bgfx_platfrom_window_{ nullptr, nullptr };
    Logger logger_{};
};

VT_END_NAMESPACE
