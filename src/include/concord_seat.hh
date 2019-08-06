
#ifndef CONCORD_SEAT_HH
#define CONCORD_SEAT_HH

#include <cstdint>

#include <wayland-server.h>

#include "concord_cursor.hh"
#include "concord_global.hh"
#include "concord_wayland_signal.hh"

extern "C"
{
#define static
#include <wlr/types/wlr_seat.h>
#undef static
}

namespace concord
{
    struct server;

    class seat
    {
    public:
        seat(server &concord_server);

    private:
        void on_cursor_moved(const cursor::motion_type, const double x, const double y);
        void on_mouse_button_pressed(const std::uint32_t time_msec, const std::uint32_t button);
        void on_mouse_button_released(const std::uint32_t time_msec, const std::uint32_t button);
        void on_cursor_axis_changed(const std::uint32_t time_msec,
                                    const wlr_axis_orientation orientation,
                                    const double delta,
                                    const std::int32_t discrete_delta,
                                    const wlr_axis_source source);
        void on_cursor_frame_changed();

    private:
        void process_cursor_motion();

    private:
        server &server_;
        cursor cursor_;
        wayland_ptr<wlr_seat> handle_;
    };
} // namespace concord

#endif /* !CONCORD_SEAT_HH */
