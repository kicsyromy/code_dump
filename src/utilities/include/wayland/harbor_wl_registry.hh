#ifndef HARBOR_UTILITIES_WL_REGSTRY_HH
#define HARBOR_UTILITIES_WL_REGSTRY_HH

#include <cassert>
#include <cstring>

#include <wayland-client.h>
#include <wayland-cursor.h>

#include "xdg-shell-unstable-v6-protocol.h"

#include "harbor_signal.hh"
#include "harbor_utilities.hh"

#undef HARBOR_LOGGER_COMPONENT
#define HARBOR_LOGGER_COMPONENT "utilities: wayland"
#include "harbor_logger.hh"

namespace harbor::utilities
{
    namespace wl
    {
        class registry
        {
        private:
            using handle_t = shared_ptr_t<wl_registry>;

            using global_listener_t = decltype(wl_registry_listener::global);
            using global_remove_listener_t = decltype(wl_registry_listener::global_remove);
            using seat_capabalities_listener_t = decltype(wl_seat_listener::capabilities);
            using seat_name_listener_t = decltype(wl_seat_listener::name);

        public:
            using wayland_display_t = shared_ptr_t<wl_display>;
            using wayland_compositor_t = shared_ptr_t<wl_compositor>;
            using wayland_seat_t = shared_ptr_t<wl_seat>;
            using wayland_shm_t = shared_ptr_t<wl_shm>;
            using wayland_cursor_theme_t = shared_ptr_t<wl_cursor_theme>;
            using wayland_cursor_t = wl_cursor *;
            using xdg_shell_t = shared_ptr_t<zxdg_shell_v6>;

        public:
            registry() noexcept
            {
                wayland_display_.reset(wl_display_connect(nullptr), [](wl_display *display) {
                    wl_display_flush(display);
                    wl_display_disconnect(display);
                });
                assert(wayland_display_ != nullptr);

                handle_.reset(wl_display_get_registry(wayland_display_.get()),
                              &wl_registry_destroy);
                const auto *wayland_registry = handle_.get();
                assert(wayland_registry != nullptr);

                wl_registry_add_listener(handle_.get(), &registry_listener, this);
                const auto display_server_roundtrip = wl_display_roundtrip(wayland_display_.get());
                assert(display_server_roundtrip > -1);
            }

        public:
            auto wayland_display() const noexcept { return weak_ref(wayland_display_); }
            auto wayland_display() noexcept { return weak_ref(wayland_display_); }
            auto wayland_compositor() const noexcept { return weak_ref(wayland_compositor_); }
            auto wayland_compositor() noexcept { return weak_ref(wayland_compositor_); }
            auto wayland_seat() const noexcept { return weak_ref(wayland_seat_); }
            auto wayland_seat() noexcept { return weak_ref(wayland_seat_); }
            auto xdg_shell() const noexcept { return weak_ref(xdg_shell_); }
            auto xdg_shell() noexcept { return weak_ref(xdg_shell_); }

        signals:
            signal(seat_capabilities_changed, std::uint32_t);

        private:
            static void handle_global(registry *self,
                                      wl_registry *registry,
                                      std::uint32_t name,
                                      const char *interface,
                                      std::uint32_t version) noexcept
            {
                if (std::strcmp(interface, wl_compositor_interface.name) == 0)
                {
                    self->wayland_compositor_.reset(
                        static_cast<wl_compositor *>(wl_registry_bind(
                            registry, name, &wl_compositor_interface, std::min(version, 4u))),
                        &wl_compositor_destroy);
                    assert(self->wayland_compositor_ != nullptr);
                }
                else if (std::strcmp(interface, wl_seat_interface.name) == 0)
                {
                    self->wayland_seat_.reset(static_cast<struct wl_seat *>(wl_registry_bind(
                                                  registry, name, &wl_seat_interface, 1)),
                                              &wl_seat_destroy);
                    assert(self->wayland_seat_ != nullptr);

                    wl_seat_add_listener(self->wayland_seat_.get(), &self->seat_listener, self);
                }
                else if (std::strcmp(interface, wl_shm_interface.name) == 0)
                {
                    self->wayland_shm_.reset(static_cast<struct wl_shm *>(wl_registry_bind(
                                                 registry, name, &wl_shm_interface, 1)),
                                             &wl_shm_destroy);
                    assert(self->wayland_shm_ != nullptr);

                    self->wayland_cursor_theme_.reset(
                        wl_cursor_theme_load(nullptr, 32, self->wayland_shm_.get()),
                        &wl_cursor_theme_destroy);

                    if (!self->wayland_cursor_theme_)
                    {
                        LOG_ERROR("Failed to load default cursor theme");
                        return;
                    }

                    self->wayland_cursor_ =
                        wl_cursor_theme_get_cursor(self->wayland_cursor_theme_.get(), "left_ptr");
                    if (!self->wayland_cursor_)
                    {
                        LOG_ERROR("Failed to load left cursor icon from default theme");
                    }
                }
                else if (std::strcmp(interface, zxdg_shell_v6_interface.name) == 0)
                {
                    self->xdg_shell_.reset(static_cast<struct zxdg_shell_v6 *>(wl_registry_bind(
                                               registry, name, &zxdg_shell_v6_interface, 1)),
                                           &zxdg_shell_v6_destroy);
                    assert(self->xdg_shell_ != nullptr);

                    zxdg_shell_v6_add_listener(self->xdg_shell_.get(), &self->xdg_shell_listener,
                                               nullptr);
                }
            }

            static void handle_global_remove([[maybe_unused]] registry *self,
                                             [[maybe_unused]] wl_registry *registry,
                                             [[maybe_unused]] std::uint32_t name) noexcept
            {
                LOG_DEBUG("Some globals were removed");
            }

            const wl_registry_listener registry_listener{
                function_cast<global_listener_t>(&handle_global),
                function_cast<global_remove_listener_t>(&handle_global_remove)
            };

            static void handle_seat_capabilities(registry *self,
                                                 [[maybe_unused]] wl_seat *wl_seat,
                                                 std::uint32_t capabilities)
            {
                self->emit_seat_capabilities_changed(capabilities);
            }

            static void handle_seat_name([[maybe_unused]] registry *self,
                                         [[maybe_unused]] wl_seat *wl_seat,
                                         [[maybe_unused]] const char *name)
            {
                LOG_DEBUG("Seat names may have changed");
            }

            const wl_seat_listener seat_listener{
                function_cast<seat_capabalities_listener_t>(&handle_seat_capabilities),
                function_cast<seat_name_listener_t>(&handle_seat_name)
            };

            const zxdg_shell_v6_listener xdg_shell_listener{
                [](void *, zxdg_shell_v6 *shell, std::uint32_t serial) {
                    zxdg_shell_v6_pong(shell, serial);
                }
            };

        private:
            wayland_display_t wayland_display_{ nullptr };
            wayland_compositor_t wayland_compositor_{ nullptr };
            wayland_seat_t wayland_seat_{ nullptr };
            wayland_shm_t wayland_shm_{ nullptr };
            wayland_cursor_theme_t wayland_cursor_theme_{ nullptr };
            wayland_cursor_t wayland_cursor_{ nullptr };

            xdg_shell_t xdg_shell_{ nullptr };

        private:
            handle_t handle_{ nullptr };
        };
    } // namespace wl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_WL_REGSTRY_HH */
