#ifndef CONCORD_OUTPUT_HH
#define CONCORD_OUTPUT_HH

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_output.h>
#undef static
}

namespace concord
{
    struct server;

    struct output
    {
        concord::server *server;
        wlr_output *handle;
        wl_listener frame;
    };
} // namespace concord

#endif /* !CONCORD_OUTPUT_HH */
