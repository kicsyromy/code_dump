#ifndef CONCORD_RENDERER_HH
#define CONCORD_RENDERER_HH

#include <ctime>

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_output.h>
#undef static
}

namespace concord
{
    struct surface;

    struct render_data
    {
        void render_surface(wlr_surface *surface, int sx, int sy, void *data);
        static void output_frame(wl_listener *listener, void *data);

        wlr_output *output;
        wlr_renderer *renderer;
        concord::surface *view;
        timespec *when;
    };
} // namespace concord

#endif /* !CONCORD_RENDERER_HH */
