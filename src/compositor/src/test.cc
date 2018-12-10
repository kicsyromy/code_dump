#include "egl/harbor_egl_surface.hh"
#include "wayland/harbor_wl_registry.hh"
#include "xdg/harbor_xdg_surface.hh"

#undef HARBOR_LOGGER_COMPONENT
#define HARBOR_LOGGER_COMPONENT "compositor: test"
#include "harbor_logger.hh"

using namespace harbor;
using namespace harbor::utilities;

static void on_seat_capabilities_changed(std::uint32_t capabilities, void *) noexcept
{
    LOG_DEBUG("Signal emited and caught: %d", capabilities);
}

void run_test() noexcept
{
    struct dummy
    {
    };
    dummy d;

    wl::registry wayland_registry;
    wayland_registry.on_seat_capabilities_changed(&d, &on_seat_capabilities_changed);

    egl::display egl_display{ wayland_registry.wayland_display() };
    egl::config egl_config{ egl_display };
    egl::context egl_context{ egl_config };
    egl::surface egl_surface{ egl_context, wayland_registry.wayland_compositor() };
    xdg::surface xdg_surface{ egl_surface, wayland_registry.xdg_shell() };
}
