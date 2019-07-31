#include "concord_server.hh"

extern "C"
{
#define static
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#undef static
}

using namespace concord;

server::server()
  /* The Wayland display is managed by libwayland. It handles accepting
     * clients from the Unix socket, manging Wayland globals, and so on. */
  : display{ wl_display_create() }
    /* The backend is a wlroots feature which abstracts the underlying input and
     * output hardware. The autocreate option will choose the most suitable
     * backend based on the current environment, such as opening an X11 window
     * if an X11 server is running. The NULL argument here optionally allows you
     * to pass in a custom renderer if wlr_renderer doesn't meet your needs. The
     * backend uses the renderer, for example, to fall back to software cursors
     * if the backend does not support hardware cursors (some older GPUs
     * don't). */
    ,
    backend{ wlr_backend_autocreate(display, nullptr) }
    /* If we don't provide a renderer, autocreate makes a GLES2 renderer for us.
     * The renderer is responsible for defining the various pixel formats it
     * supports for shared memory, this configures that for clients. */
    ,
    renderer{ wlr_backend_get_renderer(backend) }

    /* Creates an output layout, which a wlroots utility for working with an
     * arrangement of screens in a physical layout. */
    ,
    output_layout{ wlr_output_layout_create() }, new_output{ backend->events.new_output }
    /* Set up the xdg-shell. The xdg-shell is a Wayland
     * protocol which is used for application windows. For more detail on
     * shells, refer to the article:
     *
     * https://drewdevault.com/2018/07/29/Wayland-shells.html
     */
    ,
    xdg_shell{ wlr_xdg_shell_create(display) },
    new_xdg_surface{ xdg_shell->events.new_surface }, cursor{ wlr_cursor_create() }
    /* Creates an xcursor manager, another wlroots utility which loads up
     * Xcursor themes to source cursor images from and makes sure that cursor
     * images are available at all scale factors on the screen (necessary for
     * HiDPI support). */
    ,
    cursor_mgr{ wlr_xcursor_manager_create(nullptr, 24) }, cursor_motion(cursor->events.motion),
    cursor_motion_absolute(cursor->events.motion_absolute), cursor_button(cursor->events.button),
    cursor_axis(cursor->events.axis), cursor_frame(cursor->events.frame)
{
    wlr_renderer_init_wl_display(renderer, display);

    /* This creates some hands-off wlroots interfaces. The compositor is
     * necessary for clients to allocate surfaces and the data device manager
     * handles the clipboard. Each of these wlroots interfaces has room for you
     * to dig your fingers in and play with their behavior if you want. */
    wlr_compositor_create(display, renderer);
    wlr_data_device_manager_create(display);

    /* Configure a listener to be notified when new outputs are available on the
     * backend. */
    new_output.connect(this, &server::on_new_output);

    /* Configure a listener to be notified when new surfaces appear */
    new_xdg_surface.connect(this, &server::on_new_xdg_surface);

    /*
     * Attach the cursor to the output layout.
     * The cursor is a wlroots utility for tracking the cursor image shown on screen.
     */
    wlr_cursor_attach_output_layout(cursor, output_layout);

    /*  We add a cursor theme at scale factor 1 to begin with. */
    wlr_xcursor_manager_load(cursor_mgr, 1);
    cursor_motion.connect(this, &server::on_cursor_motion);
    cursor_motion_absolute.connect(this, &server::on_cursor_motion_absolute);
    cursor_button.connect(this, &server::on_cursor_button);
    cursor_axis.connect(this, &server::on_cursor_axis);
    cursor_frame.connect(this, &server::on_cursor_frame);
}

bool server::handle_keybinding(xkb_keysym_t sym) {}

void server::server_new_keyboard(wlr_input_device *device) {}

void server::server_new_pointer(wlr_input_device *device) {}

void server::server_new_input(void *data) {}

view *server::desktop_view_at(
    double lx, double ly, struct wlr_surface **surface, double *sx, double *sy)
{
}

void server::process_cursor_move(std::uint32_t time) {}

void server::process_cursor_resize(std::uint32_t time) {}

void server::process_cursor_motion(std::uint32_t time) {}

void server::seat_request_cursor(wl_listener *listener, void *data) {}

void server::on_cursor_motion(wlr_event_pointer_motion &event) {}

void server::on_cursor_motion_absolute(wlr_event_pointer_motion_absolute &event) {}

void server::on_cursor_button(wlr_event_pointer_button &event) {}

void server::on_cursor_axis(wlr_event_pointer_axis &event) {}

void server::on_cursor_frame(char &) {}

void server::on_new_output(wlr_output &output) {}

void server::on_new_xdg_surface(wlr_xdg_surface &surface) {}
