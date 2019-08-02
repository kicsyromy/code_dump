#ifndef CONCORD_WAYLAND_SIGNAL_HH
#define CONCORD_WAYLAND_SIGNAL_HH

#include <wayland-server.h>

#include "concord_global.hh"
#include "concord_signal.hh"

namespace concord
{
    namespace wayland
    {
        template <typename Arg = std::nullptr_t> class signal : public std::conditional_t<!std::is_same_v<Arg, std::nullptr_t>, events::signal<Arg &>, events::signal<>>
        {
        private:
            using this_t = signal<Arg>;

        public:
            inline signal(wl_signal &signal) noexcept
            {
                handle_.notify = &signal::on_wl_signal_triggered;
                wl_signal_add(&signal, &handle_);
            }

        private:
            static void on_wl_signal_triggered(wl_listener *listener, [[maybe_unused]] void *data)
            {
                this_t *self = wl_container_of(listener, self, handle_);
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
            wl_listener handle_{};

        private:
            DISABLE_COPY(signal);
            DISABLE_MOVE(signal);
        };
    } // namespace wayland
} // namespace concord

#endif /* !CONCORD_EVENTS_WAYLAND_SIGNAL_HH */
