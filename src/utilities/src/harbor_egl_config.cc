#include "harbor_egl_config.hh"
#include "harbor_egl_config_private.hh"

#include <cassert>

#include <utility>
#include <vector>

#include "harbor_egl_surface.hh"
#include "harbor_utilities.hh"

#define HARBOR_LOGGER_COMPONENT "utilities::egl::config"
#include "harbor_logger.hh"

using namespace harbor::utilities;

namespace
{
    constexpr auto context_attributes = make_array(EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE);
    constexpr auto configuration_attributes = make_array(EGL_SURFACE_TYPE,
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
    constexpr auto extentions = make_array(
        std::make_pair("EGL_EXT_swap_buffers_with_damage", "eglSwapBuffersWithDamageEXT"),
        std::make_pair("EGL_KHR_swap_buffers_with_damage", "eglSwapBuffersWithDamageKHR"));

} // namespace

egl::config_private::config_private() noexcept
{
    auto wayland_display = force_cast<EGLNativeDisplayType>(wl_display_connect(nullptr));
    assert(wayland_display != nullptr);

    egl_display_ = eglGetDisplay(wayland_display);
    assert(egl_display_ != nullptr);

    EGLint version_major{ 0 };
    EGLint version_minor{ 0 };
    const auto initialization = eglInitialize(egl_display_, &version_major, &version_minor);
    assert(initialization == EGL_TRUE);

    const auto gs_api_binding = eglBindAPI(EGL_OPENGL_ES_API);
    assert(gs_api_binding == EGL_TRUE);

    EGLint config_size{ 0 };
    const auto get_egl_configurations = eglGetConfigs(egl_display_, nullptr, 0, &config_size);
    assert(get_egl_configurations == EGL_TRUE);
    assert(config_size > 0);

    std::vector<EGLConfig> available_configurations(static_cast<std::size_t>(config_size), nullptr);
    EGLint config_count{ 0 };
    const auto choose_egl_configuration =
        eglChooseConfig(egl_display_, configuration_attributes.data(),
                        available_configurations.data(), config_size, &config_count);
    assert(choose_egl_configuration == EGL_TRUE);
    assert(config_count > 0);

    EGLint egl_buffer_size{ 0 };
    for (const auto &configuration : available_configurations)
    {
        eglGetConfigAttrib(egl_display_, configuration, EGL_BUFFER_SIZE, &egl_buffer_size);
        if (egl_buffer_size == surface::COLOR_BUFFER_SIZE)
        {
            configuration_ = configuration;
        }
    }
    assert(configuration_ != nullptr);

    context_ =
        eglCreateContext(egl_display_, configuration_, EGL_NO_CONTEXT, context_attributes.data());
    assert(context_ != nullptr);

    const auto available_extensions = eglQueryString(egl_display_, EGL_EXTENSIONS);
    if (available_extensions != nullptr)
    {
        for (const auto &extension_id : extentions)
        {
            auto extension_function = eglGetProcAddress(extension_id.second);
            if (extension_function != nullptr)
            {
                auto fn = reinterpret_cast<PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC>(extension_function);
                swap_buffers_function =
                    [ this, fn ](EGLSurface surface, EGLint * rects, EGLint n_rects) noexcept
                {
                    fn(egl_display_, surface, rects, n_rects);
                };
                break;
            }
        }
    }

    if (swap_buffers_function == nullptr)
    {
        swap_buffers_function = [this](EGLSurface surface, EGLint *, EGLint) noexcept
        {
            eglSwapBuffers(egl_display_, surface);
        };
    }
}

egl::config::config() noexcept : private_{ std::make_unique<egl::config_private>() }
{
    swap_buffers_function = private_->swap_buffers_function;
}
