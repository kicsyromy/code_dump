#ifndef VK_SURFACE_H
#define VK_SURFACE_H

#include "support.h"
#include "vk_instance.h"
#include "vk_window.h"
#include "logger.h"

namespace vk
{
    class surface_t
    {
    public:
        inline surface_t(
                const vk::instance_t &vk_instance,
                const vk::window_t &vk_window)
          : vk_instance_(vk_instance)
        {
            if (glfwCreateWindowSurface(
                        vk_instance,
                        vk_window,
                        nullptr,
                        &handle_) != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create window surface");
            }
        }

        inline ~surface_t()
        {
            vkDestroySurfaceKHR(vk_instance_, handle_, nullptr);
        }

    public:
        inline operator VkSurfaceKHR() const { return handle_; }

    private:
        VkSurfaceKHR handle_ { nullptr };
        const vk::instance_t &vk_instance_;

    private:
        DISABLE_COPY(surface_t)
        DISABLE_MOVE(surface_t)
    };
}

#endif // !VK_SURFACE_H
