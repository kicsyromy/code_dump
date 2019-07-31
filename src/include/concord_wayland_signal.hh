#ifndef CONCORD_WAYLAND_SIGNAL_HH
#define CONCORD_WAYLAND_SIGNAL_HH

#include <wayland-server.h>

#include "concord_signal.hh"

namespace concord
{
    namespace wayland
    {
        template <typename Arg> class signal : public events::signal<Arg &>
        {
        public:
            inline signal(wl_signal &signal) noexcept
            {
                handle_.notify = &signal::on_wl_signal_triggered;
                wl_signal_add(&signal, &handle_);
            }

        private:
            static void on_wl_signal_triggered(wl_listener *listener, void *data)
            {
                signal *self = wl_container_of(listener, self, handle_);
                self->emit(*static_cast<Arg *>(data));
            }

        private:
            wl_listener handle_{};
        };
    } // namespace wayland
} // namespace concord

#endif /* !CONCORD_EVENTS_WAYLAND_SIGNAL_HH */
