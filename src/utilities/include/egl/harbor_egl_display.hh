#ifndef HARBOR_UTILITIES_EGL_DISPLAY_HH
#define HARBOR_UTILITIES_EGL_DISPLAY_HH

#include <cassert>

#include <EGL/egl.h>

#include <wayland-client.h>

#include "harbor_utilities.hh"

namespace harbor::utilities
{
    namespace egl
    {
        class display
        {
        private:
            using handle_t = shared_ptr_t<void>;

        private:
            static constexpr auto EGL_API{ EGL_OPENGL_ES_API };

        public:
            display(weak_ptr_t<wl_display> wayland_display) noexcept
            {
                wayland_display_ = wayland_display.lock();
                assert(wayland_display_ != nullptr);

                auto native_display = force_cast<EGLNativeDisplayType>(wayland_display_.get());
                assert(native_display != nullptr);

                handle_.reset(eglGetDisplay(native_display), [](EGLDisplay display) noexcept {
                    eglTerminate(display);
                    eglReleaseThread();
                });
                assert(handle_ != nullptr);

                [[maybe_unused]] EGLint version_major{ 0 };
                [[maybe_unused]] EGLint version_minor{ 0 };
                const auto initialization =
                    eglInitialize(handle_.get(), &version_major, &version_minor);
                assert(initialization == EGL_TRUE);

                const auto gs_api_binding = eglBindAPI(EGL_API);
                assert(gs_api_binding == EGL_TRUE);
            }

        public:
            [[nodiscard]] handle_t::element_type *native_handle() const noexcept
            {
                return handle_.get();
            }

        private:
            shared_ptr_t<wl_display> wayland_display_{ nullptr };

        private:
            handle_t handle_{ nullptr };
        };
    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_DISPLAY_HH */
