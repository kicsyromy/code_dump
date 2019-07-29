#ifndef CONCORD_SERVER_HH
#define CONCORD_SERVER_HH

#include <cstdint>

#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>

extern "C"
{
#define static
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#undef static
}

#include "concord_cursor.hh"
#include "concord_view.hh"

namespace concord
{
    struct server
    {
        bool handle_keybinding(xkb_keysym_t sym);
        void server_new_keyboard(wlr_input_device* device);
        void server_new_pointer(wlr_input_device* device);
        void server_new_input(void* data);
        view* desktop_view_at(double lx,
            double ly,
            struct wlr_surface** surface,
            double* sx,
            double* sy);
        void process_cursor_move(std::uint32_t time);
        void process_cursor_resize(std::uint32_t time);
        void process_cursor_motion(std::uint32_t time);

        static void seat_request_cursor(wl_listener* listener, void* data);
        static void server_cursor_motion(wl_listener* listener, void* data);
        static void server_cursor_motion_absolute(wl_listener* listener, void* data);
        static void server_cursor_button(wl_listener* listener, void* data);
        static void server_cursor_axis(wl_listener* listener, void* data);
        static void server_cursor_frame(wl_listener* listener, void* data);
        static void server_new_output(wl_listener* listener, void* data);
        static void server_new_xdg_surface(wl_listener* listener, void* data);

        wl_display *display;
        wlr_backend *backend;
        wlr_renderer *renderer;

        wlr_xdg_shell *xdg_shell;
        wl_listener new_xdg_surface;
        wl_list views;

        wlr_cursor *cursor;
        wlr_xcursor_manager *cursor_mgr;
        wl_listener cursor_motion;
        wl_listener cursor_motion_absolute;
        wl_listener cursor_button;
        wl_listener cursor_axis;
        wl_listener cursor_frame;

        wlr_seat *seat;
        wl_listener new_input;
        wl_listener request_cursor;
        wl_list keyboards;
        concord::cursor_mode cursor_mode;
        view *grabbed_view;
        double grab_x, grab_y;
        int grab_width, grab_height;
        std::uint32_t resize_edges;

        wlr_output_layout *output_layout;
        wl_list outputs;
        wl_listener new_output;
    };
}

#endif /* !CONCORD_SERVER_HH */
