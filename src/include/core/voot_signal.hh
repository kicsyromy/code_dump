#pragma once

#include "voot_global.hh"
#include "core/voot_lifeline.hh"
#include "core/voot_logger.hh"

#include <functional>
#include <vector>
#include <variant>

VT_BEGIN_NAMESPACE

template<typename... Args> class Signal
{
    VT_ADD_LIFELINE

public:
    template<auto Slot, typename Receiver = typename utility::GetClassType<decltype(Slot)>::Type>
    void connect(Receiver &object)
    {

        constexpr auto cb = [](Args... args, void *obj, const Weakline &lifeline) {
            static_cast<void>(lifeline);

            auto *receiver = static_cast<Receiver *>(obj);

            if constexpr (has_lifeline<Receiver>())
            {
                if (lifeline.expired())
                {
                    VT_LOG_ERROR("Tried to send signal to destroyed object {}", obj);
                    return;
                }
            }

            (receiver->*Slot)(std::forward<Args>(args)...);
        };

        if constexpr (has_lifeline<Receiver>())
        {
            connections_.emplace_back(Connection{ &object, cb, object.get_lifeline() });
        }
        else
        {
            connections_.emplace_back(Connection{ &object, cb });
        }
    }

    template<void (*Slot)(Args...)> void connect()
    {
        connections_.emplace_back(
            Connection{ static_cast<void *>(nullptr), [](Args... args, void *, const Weakline &) {
                           Slot(std::forward<Args>(args)...);
                       } });
    }

    template<typename Slot, std::enable_if_t<std::is_invocable_r_v<void, Slot, Args...>, int> = 0>
    void connect(Slot &&slot)
    {
        connections_.emplace_back(std::forward<Slot>(slot));
    }

    void connect(Signal<Args...> &other)
    {
        constexpr auto cb = [](Args... args, void *s, const Weakline &lifeline) {
            if (lifeline.expired())
            {
                VT_LOG_ERROR("Tried to send signal to destroyed signal");
                return;
            }
            static_cast<Signal<Args...> *>(s)->emit(std::forward<Args>(args)...);
        };
        connections_.emplace_back(
            Connection{ static_cast<void *>(&other), cb, other.get_lifeline() });
    }

    void emit(Args... args) const
    {
        for (auto &connection : connections_)
        {
            const auto visitor = [&args...](auto &conn) {
                if constexpr (std::is_same_v<std::decay_t<decltype(conn)>, Connection>)
                {
                    conn.slot()(std::forward<Args>(args)..., conn.object(), conn.lifeline());
                }
                if constexpr (std::is_same_v<std::decay_t<decltype(conn)>,
                                  std::function<void(Args...)>>)
                {
                    conn(std::forward<Args>(args)...);
                }
            };
            std::visit(visitor, connection);
        }
    }

private:
    struct Connection
    {
        using Slot = void (*)(Args..., void *, const Weakline &);

        inline Connection(void *object, Slot &&slot, Weakline lifeline = {}) noexcept
          : object_{ object }
          , slot_{ slot }
          , lifeline_{ lifeline }
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

        inline const Weakline &lifeline() const noexcept
        {
            return lifeline_;
        }

    private:
        void *object_;
        Slot slot_;
        Weakline lifeline_;
    };

    using ConnectionVariant = std::variant<std::function<void(Args...)>, Connection>;

private:
    std::vector<ConnectionVariant> connections_;
};

VT_END_NAMESPACE
