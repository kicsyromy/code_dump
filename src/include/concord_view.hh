#ifndef CONCORD_VIEW_HH
#define CONCORD_VIEW_HH

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_xdg_shell.h>
#undef static
}

#include "concord_cursor.hh"

namespace concord
{
    struct server;

    struct view
    {
        void focus_view(wlr_surface* surface);
        bool view_at(double lx,
            double ly,
            struct wlr_surface** surface,
            double* sx,
            double* sy);
        void begin_interactive(cursor_mode mode, uint32_t edges);

        static void xdg_surface_map(wl_listener* listener, void* data);
        static void xdg_surface_unmap(wl_listener *listener, void *data);
        static void xdg_surface_destroy(wl_listener *listener, void *data);
        static void xdg_toplevel_request_move(wl_listener* listener, void* data);
        static void xdg_toplevel_request_resize(wl_listener* listener, void* data);

        concord::server *server;

        wl_list link;
        wlr_xdg_surface *xdg_surface;
        wl_listener map;
        wl_listener unmap;
        wl_listener destroy;
        wl_listener request_move;
        wl_listener request_resize;
        bool mapped;
        int x, y;
    };
}

#endif /* !CONCORD_VIEW_HH */
