#ifndef CONCORD_SERVER_HH
#define CONCORD_SERVER_HH

#include <vector>

#include <cstdint>

#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>

extern "C"
{
#define static
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#undef static
}

#include "concord_cursor.hh"
#include "concord_output.hh"
#include "concord_view.hh"
#include "concord_wayland_signal.hh"

namespace concord
{
    struct server
    {
    public:
        server();

    public:
        bool handle_keybinding(xkb_keysym_t sym);
        void server_new_keyboard(wlr_input_device *device);
        void server_new_pointer(wlr_input_device *device);
        void server_new_input(void *data);
        view *desktop_view_at(
            double lx, double ly, struct wlr_surface **surface, double *sx, double *sy);
        void process_cursor_move(std::uint32_t time);
        void process_cursor_resize(std::uint32_t time);
        void process_cursor_motion(std::uint32_t time);

    public:
        void seat_request_cursor(wl_listener *listener, void *data);
        void on_cursor_motion(wlr_event_pointer_motion &event);
        void on_cursor_motion_absolute(wlr_event_pointer_motion_absolute &event);
        void on_cursor_button(wlr_event_pointer_button &event);
        void on_cursor_axis(wlr_event_pointer_axis &event);
        void on_cursor_frame(char &event);
        void on_new_output(wlr_output &output);
        void on_new_xdg_surface(wlr_xdg_surface &surface);

    private:
        wl_display *display;
        wlr_backend *backend;
        wlr_renderer *renderer;

        wlr_output_layout *output_layout;
        std::vector<output> outputs;
        wayland::signal<wlr_output> new_output;

        wlr_xdg_shell *xdg_shell;
        wayland::signal<wlr_xdg_surface> new_xdg_surface;
        std::vector<view> views;

        wlr_cursor *cursor;
        wlr_xcursor_manager *cursor_mgr;
        wayland::signal<wlr_event_pointer_motion> cursor_motion;
        wayland::signal<wlr_event_pointer_motion_absolute> cursor_motion_absolute;
        wayland::signal<wlr_event_pointer_button> cursor_button;
        wayland::signal<wlr_event_pointer_axis> cursor_axis;
        wayland::signal<char> cursor_frame;

        wlr_seat *seat;
        wl_listener new_input;
        wl_listener request_cursor;
        wl_list keyboards;
        concord::cursor_mode cursor_mode;
        view *grabbed_view;
        double grab_x, grab_y;
        int grab_width, grab_height;
        std::uint32_t resize_edges;
    };
} // namespace concord

#endif /* !CONCORD_SERVER_HH */
