#pragma once

#include "voot_global.hh"

#include <functional>
#include <vector>

VOOT_BEGIN_NAMESPACE

template<typename... Args> class Signal
{
public:
    template<typename Receiver> void connect(Receiver &object, void (Receiver::*slot)(Args...))
    {
        connections_.emplace_back(&object, [&object, slot](Args... args) {
            object.*slot(std::forward<Args>(args)...);
        });
    }

    template<typename Callable> void connect(Callable &&slot)
    {
        static_assert(std::is_invocable_r_v<void, Callable, Args...>);
        connections_.emplace_back(static_cast<void *>(nullptr), slot);
    }

    void emit(Args... args)
    {
        for (auto &connection : connections_)
        {
            connection.slot()(std::forward<Args>(args)...);
        }
    }

private:
    struct Connection
    {
        using Slot = std::function<void(Args...)>;

        constexpr Connection(void *object, Slot &&slot) noexcept
          : object_{ object }
          , slot_{ slot }
        {}

    public:
        constexpr void *object() const noexcept
        {
            return object_;
        }

        constexpr const Slot &slot() const noexcept
        {
            return slot_;
        }

    private:
        void *object_;
        Slot slot_;
    };

private:
    std::vector<Connection> connections_;
};

VOOT_END_NAMESPACE
