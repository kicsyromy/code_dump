#ifndef CONCORD_OUTPUT_HH
#define CONCORD_OUTPUT_HH

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_output.h>
#undef static
}

namespace condord
{
    struct server;

    struct output
    {
        wl_list link;
        condord::server *server;
        wlr_output *output;
        wl_listener frame;
    };
}

#endif /* !CONCORD_OUTPUT_HH */
