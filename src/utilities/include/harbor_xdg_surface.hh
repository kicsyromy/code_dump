#ifndef HARBOR_UTILITIES_XDG_SURFACE_HH
#define HARBOR_UTILITIES_XDG_SURFACE_HH

#include "harbor_egl_surface.hh"
#include "harbor_utilities.hh"

#include "xdg-shell-unstable-v6-protocol.h"

namespace harbor::utilities
{
    namespace xdg
    {
        class surface
        {
        private:
            using handle_t = shared_ptr_t<zxdg_surface_v6>;

            using xdg_toplevel_t = shared_ptr_t<zxdg_toplevel_v6>;

        public:
            surface(const egl::surface &surface, zxdg_shell_v6 *xdg_shell) noexcept
              : surface_{ surface }, wayland_surface_{ surface.wayland_surface() }
            {
                handle_.reset(zxdg_shell_v6_get_xdg_surface(xdg_shell, wayland_surface_.get()),
                              &zxdg_surface_v6_destroy);
                // zxdg_surface_v6_add_listener(window->xdg_surface, &xdg_surface_listener, window);

                top_level_.reset(zxdg_surface_v6_get_toplevel(handle_.get()),
                                 &zxdg_toplevel_v6_destroy);
                // zxdg_toplevel_v6_add_listener(window->xdg_toplevel, &xdg_toplevel_listener, window);

                // zxdg_toplevel_v6_set_title(window->xdg_toplevel, "simple-egl");

                // window->wait_for_configure = true;
                wl_surface_commit(wayland_surface_.get());
            }

        private:
            egl::surface surface_;
            egl::surface::wl_surface_handle_t wayland_surface_{ nullptr };

            xdg_toplevel_t top_level_{ nullptr };

        private:
            handle_t handle_{ nullptr };
        };
    } // namespace xdg
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_XDG_SURFACE_HH */
