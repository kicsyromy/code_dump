#ifndef CONCORD_UTILITIES_SIGNAL_H
#define CONCORD_UTILITIES_SIGNAL_H

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <cassert>

#include <wayland-server.h>

namespace concord
{
    namespace events
    {
        template <typename... Args> class signal
        {
        public:
            inline constexpr signal() = default;

        public:
            template <typename callee_t>
            auto connect(callee_t *callee, void (callee_t::*callback)(Args...))
            {
                clients_.emplace(callee, connections_.size());
                connections_.push_back([callee, callback](Args &&... args) {
                    ((*callee).*callback)(std::forward<Args>(args)...);
                });
            }

            template <typename callee_t> void connect(callee_t *callee, void (*callback)(Args...))
            {
                assert(callee != nullptr);

                clients_.emplace(callee, connections_.size());
                connections_.push_back(callback);
            }

            void connect(void (*callback)(Args...)) { connections_.push_back(callback); }

            void emit(Args &&... args) const
            {
                for (const auto &connection : connections_)
                {
                    connection(std::forward<Args>(args)...);
                }
            }

        private:
            std::vector<std::function<void(Args...)>> connections_{};
            std::unordered_map<void *, std::size_t> clients_{};
        };

        template <typename Arg>
        class wayland_signal : public signal<Arg *>
        {
        public:
            inline wayland_signal(wl_signal &signal) noexcept
            {
                handle_.notify = &wayland_signal::on_wl_signal_triggered;
                wl_signal_add(&signal, &handle_);
            }

        private:
            static void on_wl_signal_triggered(wl_listener *listener, void *data)
            {
                wayland_signal *self = wl_container_of(listener, self, handle_);
                self->emit(static_cast<Arg *>(data));
            }

        private:
            wl_listener handle_{};
        };
    } // namespace events
} // namespace concord

#endif /* !CONCORD_UTILITIES_SIGNAL_H */
