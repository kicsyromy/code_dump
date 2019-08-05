#ifndef CONCORD_SERVER_HH
#define CONCORD_SERVER_HH

#include <list>
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
#include "concord_global.hh"
#include "concord_keyboard.hh"
#include "concord_output_layout.hh"
#include "concord_surface.hh"
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
        surface *desktop_view_at(
            double lx, double ly, struct wlr_surface **surface, double *sx, double *sy);
        void process_cursor_move(std::uint32_t time);
        void process_cursor_resize(std::uint32_t time);
        void process_cursor_motion(std::uint32_t time);

    public:
        void on_seat_request_cursor(wlr_seat_pointer_request_set_cursor_event &event);
        void on_new_input(wlr_input_device &device);

    public:
        void on_cursor_motion(wlr_event_pointer_motion &event);
        void on_cursor_motion_absolute(wlr_event_pointer_motion_absolute &event);
        void on_cursor_button(wlr_event_pointer_button &event);
        void on_cursor_axis(wlr_event_pointer_axis &event);
        void on_cursor_frame();
        void on_new_xdg_surface(wlr_xdg_surface &xdg_surface);

        wl_display *display;
        wlr_backend *backend;
        wlr_renderer *renderer;

        output_layout output_layout_;

        wayland_ptr<wlr_xdg_shell> xdg_shell_;
        wayland::signal<wlr_xdg_surface> new_xdg_surface;
        std::list<surface> views;

        wlr_cursor *cursor;
        wlr_xcursor_manager *cursor_mgr;
        wayland::signal<wlr_event_pointer_motion> cursor_motion;
        wayland::signal<wlr_event_pointer_motion_absolute> cursor_motion_absolute;
        wayland::signal<wlr_event_pointer_button> cursor_button;
        wayland::signal<wlr_event_pointer_axis> cursor_axis;
        wayland::signal<> cursor_frame;

        wlr_seat *seat;
        wayland::signal<wlr_input_device> new_input;
        wayland::signal<wlr_seat_pointer_request_set_cursor_event> request_cursor;
        std::vector<keyboard> keyboards;

        concord::cursor::mode cursor_mode;
        surface *grabbed_view;
        double grab_x, grab_y;
        int grab_width, grab_height;
        std::uint32_t resize_edges;

    private:
        void on_surface_focus_requested(surface &surface);
        void on_surface_move_requested(surface &surface);
        void on_surface_resize_requested(surface &surface, const std::uint32_t edges);
        void on_surface_destroyed(surface &surface);
    };
} // namespace concord

#endif /* !CONCORD_SERVER_HH */
