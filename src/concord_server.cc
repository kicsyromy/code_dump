#include "concord_server.hh"

extern "C"
{
#define static
#include <wlr/types/wlr_data_device.h>
#undef static
}

using namespace concord;

server::server()
{
    wlr_renderer_init_wl_display(renderer, display);
    wlr_data_device_manager_create(display);

    /* Set up the xdg-shell. The xdg-shell is a Wayland
     * protocol which is used for application windows. For more detail on
     * shells, refer to the article:
     *
     * https://drewdevault.com/2018/07/29/Wayland-shells.html
     */
    // new_xdg_surface.notify = &server::server_new_xdg_surface;
    // wl_signal_add(&xdg_shell->events.new_surface, &new_xdg_surface);
    new_xdg_surface.connect(this, &server::server_new_xdg_surface);

    /*
     * Creates a cursor, which is a wlroots utility for tracking the cursor
     * image shown on screen.
     */
    wlr_cursor_attach_output_layout(cursor, output_layout);

    /* Creates an xcursor manager, another wlroots utility which loads up
     * Xcursor themes to source cursor images from and makes sure that cursor
     * images are available at all scale factors on the screen (necessary for
     * HiDPI support). We add a cursor theme at scale factor 1 to begin with. */
    wlr_xcursor_manager_load(cursor_mgr, 1);
}

bool server::handle_keybinding(xkb_keysym_t sym)
{
}

void server::server_new_keyboard(wlr_input_device *device)
{
}

void server::server_new_pointer(wlr_input_device *device)
{
}

void server::server_new_input(void *data)
{
}

view *server::desktop_view_at(double lx, double ly, struct wlr_surface **surface, double *sx, double *sy)
{
}

void server::process_cursor_move(std::uint32_t time)
{
}

void server::process_cursor_resize(std::uint32_t time)
{
}

void server::process_cursor_motion(std::uint32_t time)
{
}

void server::seat_request_cursor(wl_listener *listener, void *data)
{
}

void server::server_cursor_motion(wl_listener *listener, void *data)
{
}

void server::server_cursor_motion_absolute(wl_listener *listener, void *data)
{
}

void server::server_cursor_button(wl_listener *listener, void *data)
{
}

void server::server_cursor_axis(wl_listener *listener, void *data)
{
}

void server::server_cursor_frame(wl_listener *listener, void *data)
{
}

void server::server_new_output(wl_listener *listener, void *data)
{
}

void server::server_new_xdg_surface(wlr_xdg_surface *surface)
{
}
