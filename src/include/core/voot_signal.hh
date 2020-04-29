#pragma once

#include "voot_global.hh"

#include <functional>
#include <vector>

VOOT_BEGIN_NAMESPACE

template<typename... Args> class Signal
{
public:
    template<auto Slot, typename Receiver = typename utility::GetClassType<decltype(Slot)>::Type>
    void connect(Receiver &object)
    {
        connections_.emplace_back(&object, [](Args... args, void *obj) {
            auto *receiver = static_cast<Receiver *>(obj);
            (receiver->*Slot)(std::forward<Args>(args)...);
        });
    }

    template<void (*Slot)(Args...)> void connect()
    {
        connections_.emplace_back(static_cast<void *>(nullptr), [](Args... args, void *) {
            Slot(std::forward<Args>(args)...);
        });
    }

    void connect(void (*Slot)(Args...))
    {
        using CallableType = decltype(Slot);
        connections_.emplace_back(reinterpret_cast<void *>(Slot), [](Args... args, void *f) {
            reinterpret_cast<CallableType>(f)(std::forward<Args>(args)...);
        });
    }

    void emit(Args... args)
    {
        for (auto &connection : connections_)
        {
            connection.slot()(std::forward<Args>(args)..., connection.object());
        }
    }

private:
    struct Connection
    {
        using Slot = void (*)(Args..., void *);

        constexpr Connection(void *object, Slot &&slot) noexcept
          : object_{ object }
          , slot_{ slot }
        {}

    public:
        constexpr void *object() const noexcept
        {
            return object_;
        }

        constexpr Slot slot() const noexcept
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
