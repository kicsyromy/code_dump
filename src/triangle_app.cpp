#include "triangle_app.h"

#include <array>

#include <cstdint>

#include "support.h"
#include "logger.h"

#include "vk_instance.h"
#include "vk_window.h"
#include "vk_debug.h"
#include "vk_surface.h"
#include "vk_physical_device.h"
#include "vk_logical_device.h"
#include "vk_queue.h"

using namespace triangle_application;

namespace
{
#ifndef NDEBUG
    constexpr const std::array validation_layers {
        "VK_LAYER_LUNARG_standard_validation"
    };
#endif

    std::vector<const char *> required_extensions()
    {
        std::vector<const char *> result;

        std::uint32_t glfw_extension_count { 0 };
        auto glfw_extensions =
                glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        for (std::uint32_t it = 0; it < glfw_extension_count; ++it)
        {
            result.push_back(glfw_extensions[it]);
        }

#ifndef NDEBUG
        result.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

        return result;
    }

    vk::instance_t create_instance()
    {
#ifndef NDEBUG
        fprintf(stderr, "Available Vulkan extensions:\n");
        const auto extensions = vk::support::supported_extensions();
        for (const auto &extension: extensions)
        {
            fprintf(stderr, "    %s\n", extension.extensionName);
        }

        if (!vk::support::check_validation_layer_support(validation_layers))
        {
            LOG_FATAL("Requested validation layers are unavailable");
        }

        return {
            "Triangle Application",
            required_extensions(),
            validation_layers
        };
#else
        return {
            "Triangle Application",
            required_extensions()
        };
#endif
    }

    bool is_device_suitable(
            const vk::physical_device_t &device,
            const vk::surface_t &surface)
    {
        auto queue_families = device.queue_families();

        int index { 0 };
        auto it = std::find_if(queue_families.cbegin(), queue_families.cend(),
                               [&](const auto &queue_family)
        {
            VkBool32 present_support { false };
            vkGetPhysicalDeviceSurfaceSupportKHR(
                        device, index, surface, &present_support);
            ++index;

            return ((queue_family.queueCount > 0) && present_support &&
                (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT));
        });

        return (it != queue_families.cend());
    }

    vk::physical_device_t pick_physical_device(
            const vk::instance_t &vk_instance,
            const vk::surface_t &vk_surface)
    {
        auto devices = vk::physical_device_t::physical_devices(vk_instance);

        for (auto &device: devices)
        {
            if (is_device_suitable(device, vk_surface))
            {
                return std::move(device);
            }
        }

        LOG_FATAL("Failed to find a suitable Vulkan capable GPU");
    }
}

void triangle_application::run()
{
    vk::window_t window;
    auto vk_instance = create_instance();
#ifndef NDEBUG
    vk::debug_t d { vk_instance };
#endif
    auto vk_surface = vk::surface_t(vk_instance, window);
    auto physical_device = pick_physical_device(vk_instance, vk_surface);

    const auto queue_graphics_family_index {
        physical_device.queue_family_index(VK_QUEUE_GRAPHICS_BIT)
    };
    const auto queue_family_surface_support_index {
        physical_device.queue_surface_support_index(vk_surface)
    };

#ifndef NDEBUG
    auto logical_device = vk::logical_device_t(
                physical_device,
                {
                    queue_graphics_family_index,
                    queue_family_surface_support_index
                },
                validation_layers);
#else
    auto logical_device = vk::logical_device_t(
                physical_device,
                {
                    queue_graphics_family_index,
                    queue_family_surface_support_index
                });
#endif
    vk::queue_t graphics_queue(logical_device,
                               queue_graphics_family_index);

    window.runMainLoop();
}
