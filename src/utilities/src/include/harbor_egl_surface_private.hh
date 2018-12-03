#ifndef HARBOR_UTILITIES_EGL_SURFACE_PRIVATE_HH
#define HARBOR_UTILITIES_EGL_SURFACE_PRIVATE_HH

#include <array>
#include <functional>
#include <memory>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <wayland-client.h>

#include "harbor_egl_config.hh"

namespace harbor::utilities
{
    namespace egl
    {
        class surface_private
        {
        public:
            surface_private() noexcept;

        private:
            EGLDisplay egl_display_{ nullptr };
            EGLContext context_{ nullptr };
            EGLConfig configuration_{ nullptr };
        };

    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_CONFIG_HH */
