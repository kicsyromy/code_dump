#ifndef CONCORD_EVENTS_SIGNAL_H
#define CONCORD_EVENTS_SIGNAL_H

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <cassert>

#include <wayland-server.h>

#include <sigc++/sigc++.h>

namespace concord
{
    namespace events
    {
        template <typename... Args> using signal = sigc::signal<void, Args...>;
        //template <typename... Args> class signal
        //{
        //public:
        //    inline constexpr signal() = default;

        //public:
        //    template <typename callee_t>
        //    auto connect(callee_t *callee, void (callee_t::*callback)(Args...))
        //    {
        //        connections_.push_back([callee, callback](Args &&... args) {
        //            ((*callee).*callback)(std::forward<Args>(args)...);
        //        });
        //    }

        //    template <typename callee_t> void connect(callee_t *callee, void (*callback)(Args...))
        //    {
        //        assert(callee != nullptr);
        //        connections_.push_back(callback);
        //    }

        //    void connect(void (*callback)(Args...)) { connections_.push_back(callback); }

        //    void emit(Args &&... args) const
        //    {
        //        for (const auto &connection : connections_)
        //        {
        //            connection(std::forward<Args>(args)...);
        //        }
        //    }

        //private:
        //    std::vector<std::function<void(Args...)>> connections_{};
        //};
    } // namespace events
} // namespace concord

#endif /* !CONCORD_EVENTS_SIGNAL_H */
