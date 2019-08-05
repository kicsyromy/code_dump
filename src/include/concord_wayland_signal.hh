#ifndef CONCORD_WAYLAND_SIGNAL_HH
#define CONCORD_WAYLAND_SIGNAL_HH

#include <functional>
#include <type_traits>

#include <wayland-server.h>

#include "concord_global.hh"
#include "concord_signal.hh"

namespace concord
{
    namespace wayland
    {
        template <typename Arg = std::nullptr_t>
        class signal : public std::conditional_t<!std::is_same_v<Arg, std::nullptr_t>,
                                                 events::signal<Arg &>,
                                                 events::signal<>>
        {
        private:
            struct wl_listener_wrapper
            {
                void *signal_instance;
                wl_listener listener;
            };

        public:
            inline signal(wl_signal &signal) noexcept : listener_{ static_cast<void *>(this), {} }
            {
                listener_.listener.notify = &signal::on_wl_signal_triggered;
                wl_signal_add(&signal, &listener_.listener);
            }

            ~signal() = default;

        public:
            signal(signal &&other) : listener_{ other.listener_ } {}
            signal &operator=(signal &&other)
            {
                listener_ = other.listener_;
                return *this;
            }

        private:
            static void on_wl_signal_triggered(wl_listener *listener, [[maybe_unused]] void *data)
            {
                wl_listener_wrapper *listener_wrapper =
                    wl_container_of(listener, listener_wrapper, listener);
                auto self = static_cast<signal *>(listener_wrapper->signal_instance);

                if constexpr (std::is_same_v<Arg, std::nullptr_t>)
                {
                    self->emit();
                }
                else
                {
                    self->emit(*static_cast<Arg *>(data));
                }
            }

        private:
            wl_listener_wrapper listener_;

        private:
            DISABLE_COPY(signal);
        };
    } // namespace wayland
} // namespace concord

#endif /* !CONCORD_EVENTS_WAYLAND_SIGNAL_HH */
