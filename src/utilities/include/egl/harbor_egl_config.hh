#ifndef HARBOR_UTILITIES_EGL_CONFIG_HH
#define HARBOR_UTILITIES_EGL_CONFIG_HH

#include <cassert>

#include <functional>
#include <memory>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <wayland-client.h>

#include "harbor_egl_display.hh"
#include "harbor_egl_types.hh"
#include "harbor_utilities.hh"

namespace harbor::utilities
{
    namespace egl
    {
        class surface;

        class config
        {
        public:
            using handle_t = EGLConfig;

        public:
            static constexpr auto CONFIGURATION_ATTRIBUTES = make_array(EGL_SURFACE_TYPE,
                                                                        EGL_WINDOW_BIT,
                                                                        EGL_RED_SIZE,
                                                                        1,
                                                                        EGL_GREEN_SIZE,
                                                                        1,
                                                                        EGL_BLUE_SIZE,
                                                                        1,
                                                                        EGL_ALPHA_SIZE,
                                                                        1,
                                                                        EGL_RENDERABLE_TYPE,
                                                                        EGL_OPENGL_ES2_BIT,
                                                                        EGL_NONE);

            static constexpr auto SURFACE_COLOR_BUFFER_SIZE{ 32 };

        public:
            config(const display &display) noexcept : display_{ display }
            {
                EGLint config_size{ 0 };
                const auto get_egl_configurations =
                    eglGetConfigs(display_.native_handle(), nullptr, 0, &config_size);
                assert(get_egl_configurations == EGL_TRUE);
                assert(config_size > 0);

                std::vector<EGLConfig> available_configurations(
                    static_cast<std::size_t>(config_size), nullptr);
                EGLint config_count{ 0 };
                const auto choose_egl_configuration =
                    eglChooseConfig(display_.native_handle(), CONFIGURATION_ATTRIBUTES.data(),
                                    available_configurations.data(), config_size, &config_count);
                assert(choose_egl_configuration == EGL_TRUE);
                assert(config_count > 0);

                EGLint egl_buffer_size{ 0 };
                for (const auto &configuration : available_configurations)
                {
                    eglGetConfigAttrib(display_.native_handle(), configuration, EGL_BUFFER_SIZE,
                                       &egl_buffer_size);
                    if (egl_buffer_size == SURFACE_COLOR_BUFFER_SIZE)
                    {
                        handle_ = configuration;
                    }
                }
                assert(handle_ != nullptr);
            }

        public:
            [[nodiscard]] inline EGLConfig native_handle() const noexcept { return handle_; }
            [[nodiscard]] inline class display display() const noexcept { return display_; }

        private:
            class display display_;

        private:
            handle_t handle_{ nullptr };
        };
    }; // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_CONFIG_HH */
