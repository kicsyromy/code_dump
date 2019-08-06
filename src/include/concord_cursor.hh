#ifndef CONCORD_CURSOR_HH
#define CONCORD_CURSOR_HH

#include <cstdint>

#include "concord_global.hh"
#include "concord_signal.hh"
#include "concord_wayland_signal.hh"

extern "C"
{
#define static
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#undef static
}

namespace concord
{
    class cursor
    {
    public:
        enum class mode
        {
            Passthrough,
            Move,
            Resize,
        };

        enum class motion_type
        {
            Absolute,
            Relative
        };

    public:
        cursor();

    public:
        events::signal<const motion_type, const double, const double> moved;
        events::signal<const std::uint32_t, const std::uint32_t> button_pressed;
        events::signal<const std::uint32_t, const std::uint32_t> button_released;
        events::signal<const std::uint32_t,
                       const wlr_axis_orientation,
                       const double,
                       const std::int32_t,
                       const wlr_axis_source>
            axis_changed;
        events::signal<> frame_event;

    private:
        void on_cursor_motion(wlr_event_pointer_motion &event);
        void on_cursor_motion_absolute(wlr_event_pointer_motion_absolute &event);
        void on_cursor_button(wlr_event_pointer_button &event);
        void on_cursor_axis(wlr_event_pointer_axis &event);
        void on_cursor_frame();

    private:
        /* Creates an xcursor manager, another wlroots utility which loads up
         * Xcursor themes to source cursor images from and makes sure that cursor
         * images are available at all scale factors on the screen (necessary for
         * HiDPI support). We add a cursor theme at scale factor 1 to begin with. */
        wayland_ptr<wlr_cursor> handle_;
        wayland_ptr<wlr_xcursor_manager> manager_;

        /*
         * wlr_cursor *only* displays an image on screen. It does not move around
         * when the pointer moves. However, we can attach input devices to it, and
         * it will generate aggregate events for all of them. In these events, we
         * can choose how we want to process them, forwarding them to clients and
         * moving the cursor around. More detail on this process is described in my
         * input handling blog post:
         *
         * https://drewdevault.com/2018/07/17/Input-handling-in-wlroots.html
         *
         * And more comments are sprinkled throughout the notify functions above.
         */
        wayland::signal<wlr_event_pointer_motion> motion;
        wayland::signal<wlr_event_pointer_motion_absolute> motion_absolute;
        wayland::signal<wlr_event_pointer_button> button;
        wayland::signal<wlr_event_pointer_axis> axis;
        wayland::signal<> frame;

    public:
        DECLARE_CONVERT_FROM_SMART_PTR(handle_)
    };
} // namespace concord

#endif /* !CONCORD_CURSOR_HH */
