#ifndef HARBOR_UTILITIES_EGL_SURFACE_HH
#define HARBOR_UTILITIES_EGL_SURFACE_HH

#include <memory>

namespace harbor::utilities
{
    namespace egl
    {
        class surface_private;

        class surface
        {
        public:
            static constexpr auto COLOR_BUFFER_SIZE{ 32 };

        public:
            surface() noexcept;

        private:
            std::unique_ptr<surface_private> private_{ nullptr };
        };

    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_SURFACE_HH */
