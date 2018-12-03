#ifndef HARBOR_UTILITIES_EGL_CONFIG_HH
#define HARBOR_UTILITIES_EGL_CONFIG_HH

#include <functional>
#include <memory>

namespace harbor::utilities
{
    namespace egl
    {
        class config_private;

        class config
        {
        public:
            config() noexcept;

        public:
            std::function<void(void *, int *, int)> swap_buffers_function{ nullptr };

        private:
            std::unique_ptr<config_private> private_{ nullptr };
        };

    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_CONFIG_HH */
