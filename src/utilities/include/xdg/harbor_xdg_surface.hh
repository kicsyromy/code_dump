#ifndef HARBOR_UTILITIES_XDG_SURFACE_HH
#define HARBOR_UTILITIES_XDG_SURFACE_HH

#include <wayland-client-protocol.h>

#include <linux/input-event-codes.h>

#include "egl/harbor_egl_surface.hh"
#include "harbor_signal.hh"
#include "harbor_utilities.hh"
#include "wayland/harbor_wl_registry.hh"

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

        private:
            using pointer_handler_enter_t = decltype(wl_pointer_listener::enter);
            using pointer_handler_leave_t = decltype(wl_pointer_listener::leave);
            using pointer_handler_motion_t = decltype(wl_pointer_listener::motion);
            using pointer_handler_button_t = decltype(wl_pointer_listener::button);
            using pointer_handler_axis_t = decltype(wl_pointer_listener::axis);
            using pointer_handler_frame_t = decltype(wl_pointer_listener::frame);
            using pointer_handler_axis_source_t = decltype(wl_pointer_listener::axis_source);
            using pointer_handler_axis_stop_t = decltype(wl_pointer_listener::axis_stop);
            using pointer_handler_axis_discrete_t = decltype(wl_pointer_listener::axis_discrete);

        public:
            using wl_pointer_t = shared_ptr_t<wl_pointer>;
            using wl_keyboard_t = shared_ptr_t<wl_keyboard>;

        public:
            enum class mouse_button
            {
                LEFT = BTN_LEFT,
                RIGHT = BTN_RIGHT,
                MIDDLE = BTN_MIDDLE
            };

        public:
            surface(const egl::surface &surface, wl::registry &wayland_registry) noexcept
              : surface_{ surface }, wayland_surface_{ surface.wayland_surface() },
                wayland_registry_{ wayland_registry }
            {
                xdg_shell_ = wayland_registry_.xdg_shell().lock();
                assert(xdg_shell_ != nullptr);

                wayland_registry_.on_seat_capabilities_changed(this,
                                                               &surface::update_seat_capabilities);

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

            [[nodiscard]] constexpr bool is_configuring() const noexcept
            {
                return wait_for_configure_;
            }

        public:
            [[nodiscard]] auto wayland_pointer() const noexcept
            {
                return weak_ref(wayland_pointer_);
            }

            [[nodiscard]] auto wayland_keyboard() const noexcept
            {
                return weak_ref(wayland_keyboard_);
            }

        signals:
            signal(mouse_entered, std::int32_t, std::int32_t);
            signal(mouse_left);
            signal(mouse_moved, std::int32_t, std::int32_t);
            signal(mouse_button_pressed, std::int32_t, std::int32_t, mouse_button);
            signal(mouse_button_released, std::int32_t, std::int32_t, mouse_button);

        private:
            static void handle_surface_configure(surface *self,
                                                 zxdg_surface_v6 *surface,
                                                 std::uint32_t serial) noexcept
            {
                zxdg_surface_v6_ack_configure(surface, serial);
                self->wait_for_configure_ = false;
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

            static void update_seat_capabilities(std::uint32_t capabilities, surface *self) noexcept
            {
                LOG_ERROR("Seat capabilities changed...");

                if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && !self->wayland_pointer_)
                {
                    self->wayland_pointer_.reset(
                        wl_seat_get_pointer(self->wayland_registry_.wayland_seat().lock().get()),
                        &wl_pointer_destroy);
                    assert(self->wayland_pointer_ != nullptr);

                    wl_pointer_add_listener(self->wayland_pointer_.get(), &self->pointer_listeners,
                                            self);
                }
                else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) &&
                         self->wayland_pointer_ != nullptr)
                {
                    self->wayland_pointer_.reset();
                }

                if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && !self->wayland_keyboard_)
                {
                    self->wayland_keyboard_.reset(
                        wl_seat_get_keyboard(self->wayland_registry_.wayland_seat().lock().get()),
                        &wl_keyboard_destroy);
                    // wl_keyboard_add_listener(d->keyboard, &keyboard_listener, d);
                }
                else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) &&
                         self->wayland_keyboard_ != nullptr)
                {
                    self->wayland_keyboard_.reset();
                }

                // if ((capabilities & WL_SEAT_CAPABILITY_TOUCH) && !d->touch)
                // {
                //     d->touch = wl_seat_get_touch(seat);
                //     wl_touch_set_user_data(d->touch, d);
                //     wl_touch_add_listener(d->touch, &touch_listener, d);
                // }
                // else if (!(capabilities & WL_SEAT_CAPABILITY_TOUCH) && d->touch)
                // {
                //     wl_touch_destroy(d->touch);
                //     d->touch = NULL;
                // }
            }

            const wl_pointer_listener pointer_listeners{
                function_cast<pointer_handler_enter_t>(&pointer_handle_enter),
                function_cast<pointer_handler_leave_t>(&pointer_handle_leave),
                function_cast<pointer_handler_motion_t>(&pointer_handle_motion),
                function_cast<pointer_handler_button_t>(&pointer_handle_button),
                function_cast<pointer_handler_axis_t>(&pointer_handle_axis),
                nullptr,
                nullptr,
                nullptr,
                nullptr
            };

            static void pointer_handle_enter(surface *self,
                                             [[maybe_unused]] struct wl_pointer *pointer,
                                             [[maybe_unsued]] std::uint32_t serial,
                                             struct wl_surface *surface,
                                             wl_fixed_t sx,
                                             wl_fixed_t sy) noexcept
            {
                if (surface == self->wayland_surface_.get())
                {
                    LOG_DEBUG("Mouse entered at: %dx%d", sx, sy);
                    self->mouse_position.x = sx;
                    self->mouse_position.y = sy;
                    self->emit_mouse_entered(sx, sy);
                }
            }

            static void pointer_handle_leave(surface *self,
                                             [[maybe_unused]] struct wl_pointer *pointer,
                                             [[maybe_unsued]] std::uint32_t serial,
                                             struct wl_surface *surface) noexcept
            {
                if (surface == self->wayland_surface_.get())
                {
                    LOG_DEBUG("Mouse left");
                    self->emit_mouse_left();
                }
            }

            static void pointer_handle_motion(surface *self,
                                              [[maybe_unused]] struct wl_pointer *pointer,
                                              [[maybe_unused]] uint32_t time,
                                              wl_fixed_t sx,
                                              wl_fixed_t sy) noexcept
            {
                LOG_DEBUG("Mouse moving at %dx%d", sx, sy);
                self->emit_mouse_moved(sx, sy);
            }

            static void pointer_handle_button(surface *self,
                                              [[maybe_unused]] struct wl_pointer *wl_pointer,
                                              [[maybe_unused]] std::uint32_t serial,
                                              [[maybe_unused]] std::uint32_t time,
                                              std::uint32_t button,
                                              std::uint32_t state) noexcept
            {
                if (state == WL_POINTER_BUTTON_STATE_PRESSED)
                {
                    LOG_DEBUG("Mouse button %d pressed at %dx%d", button, self->mouse_position.x,
                              self->mouse_position.y);
                    self->emit_mouse_button_pressed(self->mouse_position.x, self->mouse_position.y,
                                                    static_cast<mouse_button>(button));
                }
                else if (state == WL_POINTER_BUTTON_STATE_RELEASED)
                {
                    LOG_DEBUG("Mouse button %d released at %dx%d", button, self->mouse_position.x,
                              self->mouse_position.y);
                    self->emit_mouse_button_released(self->mouse_position.x, self->mouse_position.y,
                                                     static_cast<mouse_button>(button));
                }
            }

            static void pointer_handle_axis(surface *self,
                                            struct wl_pointer *wl_pointer,
                                            uint32_t time,
                                            uint32_t axis,
                                            wl_fixed_t value) noexcept
            {
            }

        private:
            egl::surface surface_;
            egl::surface::wl_surface_handle_t wayland_surface_{ nullptr };

            wl::registry &wayland_registry_;
            wl_pointer_t wayland_pointer_;
            wl_keyboard_t wayland_keyboard_;

            xdg_toplevel_t top_level_{ nullptr };
            shared_ptr_t<zxdg_shell_v6> xdg_shell_{ nullptr };

        private:
            handle_t handle_{ nullptr };

        private:
            bool wait_for_configure_{ true };
            struct
            {
                std::int32_t x{ 0 };
                std::int32_t y{ 0 };
            } mouse_position;
        };
    } // namespace xdg
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_XDG_SURFACE_HH */
