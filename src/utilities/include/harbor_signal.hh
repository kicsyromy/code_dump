#ifndef HARBOR_UTILITIES_SIGNAL_H
#define HARBOR_UTILITIES_SIGNAL_H

#include <cstdint>

#include <string>
#include <unordered_map>

#include "harbor_utilities.hh"

#define signals public
#define signal(name, ...)                                                                          \
private:                                                                                           \
    using signal_##name##_t = harbor::utilities::signal_t<__VA_ARGS__>;                            \
                                                                                                   \
    signal_##name##_t signal_##name##_;                                                            \
    template <typename... args_t> inline void emit_##name(args_t... args) const noexcept           \
    {                                                                                              \
        signal_##name##_.emit(args...);                                                            \
    }                                                                                              \
                                                                                                   \
public:                                                                                            \
    using signal_##name##_signature_t = signal_##name##_t::signature_t;                            \
    template <typename callee_t, typename callback_t, typename user_data_t = void>                 \
    inline void on_##name(callee_t *callee, callback_t handler,                                    \
                          user_data_t *user_data = nullptr) noexcept                               \
    {                                                                                              \
        signal_##name##_.connect(callee, handler, user_data ? user_data : callee);                 \
    }                                                                                              \
    inline void disconnect_##name(void *callee) noexcept { signal_##name##_.disconnect(callee); }

namespace harbor::utilities
{
    template <typename... args_t> class signal_t
    {
    public:
        using signature_t = void (*)(args_t..., void *);

    public:
        template <typename callee_t, typename callback_t, typename user_data_t>
        void connect(callee_t *callee, callback_t callback, user_data_t *user_data) noexcept
        {
            clients_.emplace(callee, connections_.size());
            connections_.push_back(std::pair{ function_cast<signature_t>(callback), user_data });
        }

        void disconnect(void *callee) noexcept
        {
            auto it = clients_.find(callee);
            if (it != clients_.end())
            {
                auto &connection = connections_[it->second];
                connection.first = nullptr;
                clients_.erase(it);
            }
        }

        void emit(args_t... args) const noexcept
        {
            for (const auto &connection : connections_)
            {
                if (connection.first != nullptr)
                {
                    connection.first(args..., connection.second);
                }
            }
        }

    private:
        std::basic_string<std::pair<signature_t, void *>> connections_{};
        std::unordered_map<void *, std::size_t> clients_{};
    };
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_SIGNAL_H */
