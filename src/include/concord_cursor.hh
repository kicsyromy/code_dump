#ifndef CONCORD_CURSOR_HH
#define CONCORD_CURSOR_HH

#include <cstdint>

extern "C"
{
#define static
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#undef static
}

#include "concord_global.hh"
#include "concord_signal.hh"
#include "concord_wayland_signal.hh"

namespace concord
{
    class cursor
    {
    public:
        enum class cursor_mode
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
        events::signal<const std::uint32_t> button_pressed;
        events::signal<const std::uint32_t> button_released;

    private:
        /* Creates an xcursor manager, another wlroots utility which loads up
         * Xcursor themes to source cursor images from and makes sure that cursor
         * images are available at all scale factors on the screen (necessary for
         * HiDPI support). We add a cursor theme at scale factor 1 to begin with. */
        WLROOTS_PTR(wlr_cursor, handle_);
        WLROOTS_PTR(wlr_xcursor_manager, manager_, nullptr, 24);

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
    };
} // namespace concord

#endif /* !CONCORD_CURSOR_HH */
