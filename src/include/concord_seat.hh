
#ifndef CONCORD_SEAT_HH
#define CONCORD_SEAT_HH

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_seat.h>
#undef static
}

namespace concord
{
    struct server;

    struct seat
    {
        concord::server *server;
        wlr_output *handle;
        wl_listener frame;
    };
} // namespace concord

#endif /* !CONCORD_SEAT_HH */
