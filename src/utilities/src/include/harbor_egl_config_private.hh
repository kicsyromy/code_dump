#ifndef HARBOR_UTILITIES_EGL_CONFIG_PRIVATE_HH
#define HARBOR_UTILITIES_EGL_CONFIG_PRIVATE_HH

#include <array>
#include <functional>
#include <memory>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <wayland-client.h>

namespace harbor::utilities
{
    namespace egl
    {
        class config_private
        {
        public:
            config_private() noexcept;

        public:
            std::function<void(EGLSurface, EGLint *, EGLint)> swap_buffers_function{ nullptr };

        private:
            EGLDisplay egl_display_{ nullptr };
            EGLContext context_{ nullptr };
            EGLConfig configuration_{ nullptr };
        };

    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_CONFIG_HH */
