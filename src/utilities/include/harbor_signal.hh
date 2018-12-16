#ifndef HARBOR_UTILITIES_SIGNAL_H
#define HARBOR_UTILITIES_SIGNAL_H

#include <cstdint>

#include <string>
#include <unordered_map>

#include "harbor_utilities.hh"

#define signals public
#define signal(name, ...)                                                                          \
private:                                                                                           \
    harbor::utilities::signal_t<__VA_ARGS__> signal_##name##_;                                     \
    template <typename... Args> inline void emit_##name(Args &&... args) const noexcept            \
    {                                                                                              \
        signal_##name##_.emit(std::forward<Args>(args)...);                                        \
    }                                                                                              \
                                                                                                   \
public:                                                                                            \
    using signal_##name##_t = decltype(signal_##name##_);                                          \
    template <typename CalleeType, typename CallbackT, typename UserDataT = void>                  \
    inline void on_##name(CalleeType *callee, CallbackT handler,                                   \
                          UserDataT *user_data = nullptr) noexcept                                 \
    {                                                                                              \
        signal_##name##_.connect(callee, handler, user_data ? user_data : callee);                 \
    }                                                                                              \
    template <typename CalleeType> inline void disconnect_##name(CalleeType *callee) noexcept      \
    {                                                                                              \
        signal_##name##_.disconnect(callee);                                                       \
    }

namespace harbor::utilities
{
    template <typename... Args> class signal_t
    {
    public:
        using signature_t = void (*)(Args..., void *);

    public:
        signal_t() = default;
        ~signal_t() = default;

    public:
        template <typename CalleeType, typename CallbackT, typename UserDataT>
        void connect(CalleeType *callee, CallbackT callback, UserDataT *user_data) noexcept
        {
            clients_.emplace(callee, connections_.size());
            connections_.push_back(
                std::pair{ function_cast<signature_t>(callback), static_cast<void *>(user_data) });
        }

        template <typename CalleeType> void disconnect(CalleeType *callee) noexcept
        {
            auto it = clients_.find(callee);
            if (it != clients_.end())
            {
                auto &connection = connections_[it->second];
                connection.first = nullptr;
                clients_.erase(it);
            }
        }

        void emit(Args... args) const noexcept
        {
            for (const auto &connection : connections_)
            {
                if (connection.first != nullptr)
                {
                    connection.first(std::forward<Args>(args)..., connection.second);
                }
            }
        }

    private:
        std::basic_string<std::pair<signature_t, void *>> connections_{};
        std::unordered_map<void *, std::size_t> clients_{};
    };
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_SIGNAL_H */
