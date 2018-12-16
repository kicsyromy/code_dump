#include <GLES2/gl2.h>

#include <nanovg.h>
#define NANOVG_GLES2_IMPLEMENTATION
#include <nanovg_gl.h>
#include <nanovg_gl_utils.h>

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
    egl_surface.make_current();

    xdg::surface xdg_surface{ egl_surface, wayland_registry };

    auto vg_context = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    auto wayland_display = wayland_registry.wayland_display().lock().get();

    while (true)
    {
        if (xdg_surface.is_configuring())
        {
            wl_display_dispatch(wayland_display);
        }
        else
        {
            wl_display_dispatch_pending(wayland_display);

            egl_surface.set_size(200, 200);

            glViewport(0, 0, 200, 200);

            glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);

            nvgBeginFrame(vg_context, 200, 200, 1);
            {
                nvgBeginPath(vg_context);
                nvgRect(vg_context, 50, 50, 100, 100);
                nvgStrokeColor(vg_context, nvgRGBA(255, 255, 255, 255));
                nvgStroke(vg_context);
            }
            nvgEndFrame(vg_context);

            glEnable(GL_DEPTH_TEST);

            egl_surface.swap_buffers();
        }
    }
}
