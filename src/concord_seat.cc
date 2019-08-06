#include "concord_seat.hh"

#include "concord_server.hh"

using namespace concord;

seat::seat(server &concord_server)
  : server_{ concord_server }, handle_{ &wlr_seat_create, &wlr_seat_destroy, server_.display,
                                        "seat0" }
{
}

void seat::on_cursor_moved(const cursor::motion_type, const double x, const double y)
{
    process_cursor_motion();
}

void seat::on_mouse_button_pressed(const std::uint32_t time_msec, const std::uint32_t button) {}

void seat::on_mouse_button_released(const std::uint32_t time_msec, const std::uint32_t button) {}

void seat::on_cursor_axis_changed(const std::uint32_t time_msec,
                                  const wlr_axis_orientation orientation,
                                  const double delta,
                                  const std::int32_t discrete_delta,
                                  const wlr_axis_source source)
{
    wlr_seat_pointer_notify_axis(handle_, time_msec, orientation, delta, discrete_delta, source);
}

void seat::on_cursor_frame_changed() { wlr_seat_pointer_notify_frame(handle_); }

void seat::process_cursor_motion() {}