#ifndef RML_UTILITIES_SIGNAL_H
#define RML_UTILITIES_SIGNAL_H

#include <cassert>

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace rml
{
    namespace events
    {
        namespace signals
        {
            enum class connection_type
            {
                Direct,
                Queued
            };
        }

        template <typename event_loop_t, typename... Args> class signal
        {
        public:
            signal(event_loop_t &event_loop) : event_loop_{ event_loop } {}

        public:
            template <typename callee_t>
            void connect(callee_t *callee,
                         void (callee_t::*callback)(Args...),
                         signals::connection_type type)
            {
                clients_.emplace(callee, connections_.size());
                connections_.emplace_back(
                    [callee, callback](Args &&... args) {
                        ((*callee).*callback)(std::forward<Args>(args)...);
                    },
                    type);
            }

            template <typename callee_t>
            void connect(callee_t *callee, void (*callback)(Args...), signals::connection_type type)
            {
                assert(callee != nullptr);

                clients_.emplace(callee, connections_.size());
                connections_.emplace_back(callback, type);
            }

            void connect(void (*callback)(Args...), signals::connection_type type)
            {
                connections_.emplace_back(callback, type);
            }

            void emit(Args &&... args)
            {
                for (const auto &connection : connections_)
                {
                    if (connection.second == signals::connection_type::Direct)
                    {
                        connection.first(std::forward<Args>(args)...);
                    }
                    else
                    {
                        emit_queued(connection.first, std::forward<Args>(args)...);
                    }
                }
            }

        private:
            struct queued_data
            {
                std::function<void(Args...)> notify;
                std::tuple<Args...> args;
                std::weak_ptr<void> lifeline;
            };

            template <typename tuple_t, size_t... I>
            inline static auto call(std::function<void(Args...)> &f,
                                    tuple_t &t,
                                    std::index_sequence<I...>) noexcept
            {
                return f(std::get<I>(t)...);
            }

            template <typename tuple_t>
            inline static auto call(std::function<void(Args...)> &f, tuple_t &t) noexcept
            {
                static constexpr auto size = std::tuple_size<tuple_t>::value;
                return call(f, t, std::make_index_sequence<size>{});
            }

            void emit_queued(const std::function<void(Args...)> &f, Args &&... args) const noexcept
            {
                auto data = new queued_data{ f, { std::forward<Args>(args)... }, lifeline_ };

                event_loop_.post(
                    [](void *d) {
                        auto data = std::unique_ptr<queued_data>{ static_cast<queued_data *>(d) };

                        if (data->lifeline.lock() != nullptr)
                        {
                            if (data->notify != nullptr)
                            {
                                call(data->notify, data->args);
                            }
                        }
                    },
                    data);
            }

        private:
            std::vector<std::pair<std::function<void(Args...)>, signals::connection_type>>
                connections_{};
            std::unordered_map<void *, std::size_t> clients_{};
            mutable std::shared_ptr<void> lifeline_{ std::make_shared<char>() };

            event_loop_t &event_loop_;
        };
    } // namespace events
} // namespace rml

#endif /* RML_UTILITIES_SIGNAL_H */
