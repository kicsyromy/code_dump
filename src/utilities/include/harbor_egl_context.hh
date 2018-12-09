#ifndef HARBOR_UTILITIES_EGL_CONTEXT_HH
#define HARBOR_UTILITIES_EGL_CONTEXT_HH

#include <EGL/egl.h>

#include "harbor_egl_config.hh"
#include "harbor_utilities.hh"

namespace harbor::utilities
{
    namespace egl
    {
        class context
        {
        private:
            using handle_t = shared_ptr_t<void>;

        private:
            static constexpr auto CONTEXT_ATTRIBUTES =
                make_array(EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE);

        public:
            context(const config &config) noexcept : display_{ config.display() }, config_{ config }
            {
                auto egl_display = display_;
                handle_.reset(eglCreateContext(display_.native_handle(), config_.native_handle(),
                                               EGL_NO_CONTEXT, CONTEXT_ATTRIBUTES.data()),
                              [display = std::move(egl_display)](EGLContext context) {
                                  eglDestroyContext(display.native_handle(), context);
                              });

                assert(handle_ != nullptr);
            }

        public:
            [[nodiscard]] inline handle_t::element_type *native_handle() const noexcept
            {
                return handle_.get();
            }
            [[nodiscard]] inline class display display() const noexcept { return display_; }
            [[nodiscard]] inline class config config() const noexcept { return config_; }

        private:
            class display display_;
            class config config_;

        private:
            handle_t handle_;
        };
    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_CONTEXT_HH */
