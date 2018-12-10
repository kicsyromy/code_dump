#ifndef HARBOR_UTILITIES_XDG_SURFACE_HH
#define HARBOR_UTILITIES_XDG_SURFACE_HH

#include "egl/harbor_egl_surface.hh"
#include "harbor_utilities.hh"

#include "xdg-shell-unstable-v6-protocol.h"

#undef HARBOR_LOGGER_COMPONENT
#define HARBOR_LOGGER_COMPONENT "utilities: xdg"
#include "harbor_logger.hh"

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
            surface(const egl::surface &surface, weak_ptr_t<zxdg_shell_v6> xdg_shell) noexcept
              : surface_{ surface }, wayland_surface_{ surface.wayland_surface() }
            {
                xdg_shell_ = xdg_shell.lock();
                assert(xdg_shell_ != nullptr);

                handle_.reset(
                    zxdg_shell_v6_get_xdg_surface(xdg_shell_.get(), wayland_surface_.get()),
                    &zxdg_surface_v6_destroy);
                zxdg_surface_v6_add_listener(handle_.get(), &xdg_surface_listener, this);

                top_level_.reset(zxdg_surface_v6_get_toplevel(handle_.get()),
                                 &zxdg_toplevel_v6_destroy);
                zxdg_toplevel_v6_add_listener(top_level_.get(), &xdg_toplevel_listener, this);

                wl_surface_commit(wayland_surface_.get());
            }

        public:
            void set_title(const std::string &title) noexcept
            {
                zxdg_toplevel_v6_set_title(top_level_.get(), title.c_str());
            }

        private:
            static void handle_surface_configure(surface *self,
                                                 zxdg_surface_v6 *surface,
                                                 std::uint32_t serial) noexcept
            {
                zxdg_surface_v6_ack_configure(surface, serial);
                self->wait_for_configure_ = true;
            }

            const zxdg_surface_v6_listener xdg_surface_listener{
                function_cast<decltype(zxdg_surface_v6_listener::configure)>(
                    &handle_surface_configure)
            };

            static void handle_toplevel_configure([[maybe_unused]] surface *self,
                                                  [[maybe_unused]] zxdg_toplevel_v6 *toplevel,
                                                  [[maybe_unused]] int32_t width,
                                                  [[maybe_unused]] int32_t height,
                                                  [[maybe_unused]] wl_array *states) noexcept
            {
                LOG_DEBUG("");
            }

            static void handle_toplevel_close(
                [[maybe_unused]] surface *self,
                [[maybe_unused]] zxdg_toplevel_v6 *xdg_toplevel) noexcept
            {
                LOG_DEBUG("");
            }

            const zxdg_toplevel_v6_listener xdg_toplevel_listener{
                function_cast<decltype(zxdg_toplevel_v6_listener::configure)>(
                    &handle_toplevel_configure),
                function_cast<decltype(zxdg_toplevel_v6_listener::close)>(&handle_toplevel_close),
            };

        private:
            egl::surface surface_;
            egl::surface::wl_surface_handle_t wayland_surface_{ nullptr };

            xdg_toplevel_t top_level_{ nullptr };
            shared_ptr_t<zxdg_shell_v6> xdg_shell_{ nullptr };

        private:
            handle_t handle_{ nullptr };

        private:
            bool wait_for_configure_{ true };
        };
    } // namespace xdg
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_XDG_SURFACE_HH */
