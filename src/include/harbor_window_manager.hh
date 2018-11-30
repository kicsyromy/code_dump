#ifndef HARBOR_WINDOW_MANAGER_HH
#define HARBOR_WINDOW_MANAGER_HH

#include <miral/canonical_window_manager.h>

namespace harbor
{
    using namespace miral;

    class WindowManagerPolicy : public CanonicalWindowManagerPolicy
    {
    public:
        using CanonicalWindowManagerPolicy::CanonicalWindowManagerPolicy;

        // Switch apps  : Alt+Tab
        // Switch window: Alt+`
        // Close window : Alt-F4
        bool handle_keyboard_event(MirKeyboardEvent const *event) override;

        // Switch apps  : click on the corresponding window
        // Switch window: click on the corresponding window
        // Move window  : Alt-leftmousebutton drag
        bool handle_pointer_event(MirPointerEvent const *event) override;

        // Switch apps  : tap on the corresponding window
        // Switch window: tap on the corresponding window
        // Move window  : three finger drag
        // Resize window: three finger pinch
        bool handle_touch_event(MirTouchEvent const *event) override;

        Rectangle confirm_placement_on_display(WindowInfo const &window_info,
                                               MirWindowState new_state,
                                               Rectangle const &new_placement) override;

        void handle_request_drag_and_drop(WindowInfo &window_info) override;

        void handle_request_move(WindowInfo &window_info,
                                 MirInputEvent const *input_event) override;

        void handle_request_resize(WindowInfo &window_info,
                                   MirInputEvent const *input_event,
                                   MirResizeEdge edge) override;

    private:
        // State held for move/resize gesture by pointer
        enum PointerGesture
        {
            pointer_gesture_none,
            pointer_gesture_moving,
            pointer_gesture_resizing
        } pointer_gesture = pointer_gesture_none;
        MirPointerButton pointer_gesture_button;
        miral::Window pointer_gesture_window;
        unsigned pointer_gesture_shift_keys = 0;
        MirResizeEdge resize_edge = mir_resize_edge_none;
        Point resize_top_left;
        Size resize_size;

        // State held for move/resize gesture by touch
        int old_touch_pinch_top = 0;
        int old_touch_pinch_left = 0;
        int old_touch_pinch_width = 0;
        int old_touch_pinch_height = 0;
        bool pinching = false;

        void end_touch_gesture();
        void keep_size_within_limits(WindowInfo const &window_info,
                                     Displacement &delta,
                                     Width &new_width,
                                     Height &new_height) const;

        bool begin_pointer_gesture(WindowInfo const &window_info,
                                   MirInputEvent const *input_event,
                                   PointerGesture gesture);
    };
} // namespace harbor

#endif /* !HARBOR_WINDOW_MANAGER_HH */
