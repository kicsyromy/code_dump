#ifndef CONCORD_KEYBOARD_HH
#define CONCORD_KEYBOARD_HH

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_input_device.h>
#undef static
}

namespace concord
{
    struct server;

    struct tinywl_keyboard
    {
        static void keyboard_handle_modifiers(wl_listener* listener, void *data);
        static void keyboard_handle_key(wl_listener* listener, void *data);

        wl_list link;
        concord::server *server;
        wlr_input_device *device;

        wl_listener modifiers;
        wl_listener key;
    };
}

#endif /* !CONCORD_KEYBOARD_HH */
