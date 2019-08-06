#include "concord_cursor.hh"

using namespace concord;

cursor::cursor()
  : handle_{ &wlr_cursor_create, &wlr_cursor_destroy },
    /*  We add a cursor theme at scale factor 1 to begin with. */
    manager_{ &wlr_xcursor_manager_create, &wlr_xcursor_manager_destroy,
              static_cast<const char *>(nullptr), 24u },
    motion{ handle_->events.motion }, motion_absolute{ handle_->events.motion_absolute },
    button{ handle_->events.button }, axis{ handle_->events.axis }, frame{ handle_->events.frame }
{
}

void cursor::on_cursor_motion(wlr_event_pointer_motion &event)
{
    wlr_cursor_move(handle_, event.device, event.delta_x, event.delta_y);
    moved.emit(motion_type::Relative, event.delta_x, event.delta_y);
}

void cursor::on_cursor_motion_absolute(wlr_event_pointer_motion_absolute &event)
{
    wlr_cursor_warp_absolute(handle_, event.device, event.x, event.y);
    moved.emit(motion_type::Absolute, event.x, event.y);
}

void cursor::on_cursor_button(wlr_event_pointer_button &event)
{
    decltype(button_pressed) &button_signal =
        event.state == WLR_BUTTON_PRESSED ? button_pressed : button_released;
    button_signal.emit(event.time_msec, event.button);
}

void cursor::on_cursor_axis(wlr_event_pointer_axis &event)
{
    axis_changed.emit(event.time_msec, event.orientation, event.delta, event.delta_discrete, event.source);
}

void cursor::on_cursor_frame() { frame_event.emit(); }
