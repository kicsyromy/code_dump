#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_output.h>
#undef static
}

#include "concord_output.hh"
#include "concord_renderer.hh"
#include "concord_server.hh"

using namespace concord;

output_layout::output::output(wlr_output *handle) : output_frame(handle->events.frame)
{
    output_frame.connect(this, &output::on_output_frame_requested);
}

void output_layout::output::on_output_frame_requested() { output_frame_requested.emit(*this); }

output_layout::output_layout(server &server)
  : server_{ server }, new_output{ server.backend->events.new_output }
{
    /* Configure a listener to be notified when new outputs are available on the
     * backend. */
    new_output.connect(this, &output_layout::on_new_output_added);
}

void output_layout::on_new_output_added(wlr_output &output)
{
    /* This event is rasied by the backend when a new output (aka a display or
     * monitor) becomes available. */

    /* Some backends don't have modes. DRM+KMS does, and we need to set a mode
     * before we can use the output. The mode is a tuple of (width, height,
     * refresh rate), and each monitor supports only a specific set of modes. We
     * just pick the first, a more sophisticated compositor would let the user
     * configure it or pick the mode the display advertises as preferred. */
    if (!wl_list_empty(&output.modes))
    {
        struct wlr_output_mode *mode = wl_container_of(output.modes.prev, mode, link);
        wlr_output_set_mode(&output, mode);
    }

    /* Allocates and configures our state for this output */
    outputs_.emplace_back(&output);

    /* Sets up a listener for the frame notify event. */
    outputs_.back().output_frame_requested.connect(this, &output_layout::on_output_frame_requested);

    /* Adds this to the output layout. The add_auto function arranges outputs
     * from left-to-right in the order they appear. A more sophisticated
     * compositor would let the user configure the arrangement of outputs in the
     * layout. */
    wlr_output_layout_add_auto(handle_.get(), &output);

    /* Creating the global adds a wl_output global to the display, which Wayland
     * clients can see to find out information about the output (such as
     * DPI, scale factor, manufacturer, etc). */
    wlr_output_create_global(&output);
}

void output_layout::on_output_frame_requested(output_layout::output &output)
{
    /* This function is called every time an output is ready to display a frame,
     * generally at the output's refresh rate (e.g. 60Hz). */
    auto renderer = server_.renderer;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    /* wlr_output_attach_render makes the OpenGL context current. */
    if (!wlr_output_attach_render(output(), nullptr))
    {
        return;
    }

    /* The "effective" resolution can change if you rotate your outputs. */
    int width, height;
    wlr_output_effective_resolution(output(), &width, &height);
    /* Begin the renderer (calls glViewport and some other GL sanity checks) */
    wlr_renderer_begin(renderer, width, height);

    const float color[4]{ 0.3f, 0.3f, 0.3f, 1.0f };
    wlr_renderer_clear(renderer, color);

    /* Each subsequent window we render is rendered on top of the last. Because
     * our view list is ordered front-to-back, we iterate over it backwards. */
    for (auto it = server_.views.rbegin(); it != server_.views.rend(); ++it)
    {
        auto &view = *it;
        if (!view.is_mapped())
        {
            /* An unmapped view should not be rendered. */
            continue;
        }
        struct render_data rdata = {
            output(),
            renderer,
            &view,
            &now,
        };
        /* This calls our render_surface function for each surface among the
         * xdg_surface's toplevel and popups. */
        wlr_xdg_surface_for_each_surface(&view(), nullptr, &rdata);
    }

    /* Hardware cursors are rendered by the GPU on a separate plane, and can be
     * moved around without re-rendering what's beneath them - which is more
     * efficient. However, not all hardware supports hardware cursors. For this
     * reason, wlroots provides a software fallback, which we ask it to render
     * here. wlr_cursor handles configuring hardware vs software cursors for you,
     * and this function is a no-op when hardware cursors are in use. */
    wlr_output_render_software_cursors(output(), nullptr);

    /* Conclude rendering and swap the buffers, showing the final frame
     * on-screen. */
    wlr_renderer_end(renderer);
    wlr_output_commit(output());
}
