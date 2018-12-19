#ifndef HARBOR_UTILITIES_EGL_SURFACE_HH
#define HARBOR_UTILITIES_EGL_SURFACE_HH

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <wayland-client.h>
#include <wayland-egl.h>

#include "harbor_egl_context.hh"
#include "harbor_egl_types.hh"
#include "harbor_utilities.hh"

namespace harbor::utilities
{
    namespace egl
    {
        class surface
        {
        private:
            using handle_t = shared_ptr_t<void>;

        public:
            using wl_window_handle_t = shared_ptr_t<wl_egl_window>;
            using wl_surface_handle_t = shared_ptr_t<wl_surface>;

        private:
            using swap_buffers_function_t = std::function<void(std::vector<rectangle> *)>;

            static constexpr auto EXTENTIONS = make_array(
                std::make_pair("EGL_EXT_swap_buffers_with_damage", "eglSwapBuffersWithDamageEXT"),
                std::make_pair("EGL_KHR_swap_buffers_with_damage", "eglSwapBuffersWithDamageKHR"));

        public:
            surface(const context &context, weak_ptr_t<wl_compositor> wayland_compositor) noexcept
              : display_{ context.display() }, context_{ context }, config_{ context.config() }
            {
                wayland_compositor_ = wayland_compositor.lock();
                assert(wayland_compositor_ != nullptr);

                wayland_surface_.reset(wl_compositor_create_surface(wayland_compositor_.get()),
                                       &wl_surface_destroy);
                assert(wayland_surface_ != nullptr);

                {
                    auto display = display_;
                    wayland_window_.reset(wl_egl_window_create(wayland_surface_.get(), 300, 300),
                                          &wl_egl_window_destroy);
                    handle_.reset(eglCreatePlatformWindowSurface(display_.native_handle(),
                                                                 config_.native_handle(),
                                                                 wayland_window_.get(), nullptr),
                                  [display = std::move(display)](EGLSurface surface) {
                                      eglMakeCurrent(display.native_handle(), nullptr, nullptr,
                                                     nullptr);
                                      eglDestroySurface(display.native_handle(), surface);
                                  });
                    const auto platform_window = handle_.get();
                    assert(platform_window != nullptr);
                }

                const auto available_extensions =
                    eglQueryString(display_.native_handle(), EGL_EXTENSIONS);
                if (available_extensions != nullptr)
                {
                    for (const auto &extension_id : EXTENTIONS)
                    {
                        auto extension_function = eglGetProcAddress(extension_id.second);
                        if (extension_function != nullptr)
                        {
                            auto fn = reinterpret_cast<PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC>(
                                extension_function);
                            swap_buffers_function_ =
                                [ this, fn ](std::vector<rectangle> * damage_regions) noexcept
                            {
                                if (damage_regions != nullptr)
                                {
                                    const auto swap_result =
                                        fn(display_.native_handle(), handle_.get(),
                                           damage_regions->data()->data.data(),
                                           static_cast<std::int32_t>(damage_regions->size()));
                                    assert(swap_result == EGL_TRUE);
                                }
                                else
                                {
                                    const auto swap_result =
                                        eglSwapBuffers(display_.native_handle(), handle_.get());
                                    assert(swap_result == EGL_TRUE);
                                }
                            };
                            break;
                        }
                    }
                }

                if (swap_buffers_function_ == nullptr)
                {
                    swap_buffers_function_ = [this](std::vector<rectangle> *) noexcept
                    {
                        const auto swap_result =
                            eglSwapBuffers(display_.native_handle(), handle_.get());
                        assert(swap_result == EGL_TRUE);
                    };
                }
                assert(swap_buffers_function_ != nullptr);

                // if (!window->frame_sync) eglSwapInterval(display->egl.dpy, 0);
                // if (window->fullscreen) zxdg_toplevel_v6_set_fullscreen(window->xdg_toplevel, NULL);
            }

        public:
            [[nodiscard]] class display display() const noexcept { return display_; }
            [[nodiscard]] class context context() const noexcept { return context_; }
            [[nodiscard]] auto wayland_surface() const noexcept
            {
                return weak_ref(wayland_surface_);
            }

        public:
            void swap_buffers(std::vector<rectangle> *damage_regions = nullptr) noexcept
            {
                swap_buffers_function_(damage_regions);
            }

            void make_current()
            {
                const auto make_current_result =
                    eglMakeCurrent(display_.native_handle(), handle_.get(), handle_.get(),
                                   context_.native_handle());
                assert(make_current_result == EGL_TRUE);
            }

            void set_size(std::int32_t width, std::int32_t height)
            {
                wl_egl_window_resize(wayland_window_.get(), width, height, 0, 0);
            }

        private:
            swap_buffers_function_t swap_buffers_function_{ nullptr };

        private:
            class display display_;
            class context context_;
            class config config_;

            shared_ptr_t<wl_compositor> wayland_compositor_{ nullptr };
            wl_surface_handle_t wayland_surface_{ nullptr };
            wl_window_handle_t wayland_window_{ nullptr };

        private:
            handle_t handle_{ nullptr };
        };

    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_SURFACE_HH */
