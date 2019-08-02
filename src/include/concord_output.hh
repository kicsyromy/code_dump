#ifndef CONCORD_OUTPUT_HH
#define CONCORD_OUTPUT_HH

#include <vector>
#include <memory>

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#undef static
}

#include "concord_global.hh"
#include "concord_signal.hh"
#include "concord_wayland_signal.hh"

namespace concord
{
    struct server;

    class output_layout
    {
    public:
        struct output
        {
            concord::server *server;
            wlr_output *handle;
            wl_listener frame;
        };

    public:
        output_layout(server &concord_server);
        ~output_layout();

    private:
        void on_new_output_added(wlr_output &output);

    private:
        server &server_;

    private:
        std::vector<output> outputs_{};
        wayland::signal<wlr_output> new_output;

    private:
      std::unique_ptr<wlr_output_layout, decltype(&wlr_output_layout_destroy)> handle_{ wlr_output_layout_create(), &wlr_output_layout_destroy };
    };
} // namespace concord

#endif /* !CONCORD_OUTPUT_HH */
