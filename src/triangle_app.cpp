#include "triangle_app.h"

#include <array>
#include <algorithm>
#include <memory>

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
#include "vk_swap_chain.h"
#include "vk_image_view.h"
#include "vk_shader.h"

using namespace triangle_application;

namespace
{
#ifndef NDEBUG
    constexpr const std::array validation_layers {
        "VK_LAYER_LUNARG_standard_validation"
    };
#endif

    const std::array required_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

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

        return (
            (it != queue_families.cend()) &&
            (device.extensions_supported(required_device_extensions)) &&
            !device.surface_formats(surface).empty() &&
            !device.surface_present_modes(surface).empty()
        );
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
                VkPhysicalDevice dev = device;
                return { dev };
            }
        }

        LOG_FATAL("Failed to find a suitable Vulkan capable GPU");
    }

    VkSurfaceFormatKHR choose_swap_surface_format(
            const vk::physical_device_t &device,
            const vk::surface_t &surface)
    {
        static constexpr auto PREFERED_FORMAT {
            VK_FORMAT_B8G8R8A8_UNORM
        };
        static constexpr auto PREFERED_COLOR_SPACE {
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        VkSurfaceFormatKHR result {
            PREFERED_FORMAT,
            PREFERED_COLOR_SPACE
        };

        auto available_formats = device.surface_formats(surface);

        if (!((available_formats.size() == 1) &&
              (available_formats.front().format == VK_FORMAT_UNDEFINED)))
        {
            auto it = std::find_if(
                          available_formats.cbegin(),
                          available_formats.cend(),
                          [] (const auto &format){
                              return (
                                  (format.format == PREFERED_FORMAT) &&
                                  (format.colorSpace == PREFERED_COLOR_SPACE)
                              );
                          }
                      );
            if (it == available_formats.cend())
            {
                result = available_formats.front();
            }
        }

        return result;
    }

    VkPresentModeKHR choose_swap_present_mode(
            const vk::physical_device_t &device,
            const vk::surface_t &surface)
    {
        VkPresentModeKHR result { VK_PRESENT_MODE_FIFO_KHR };

        auto available_modes = device.surface_present_modes(surface);
        for (const auto &mode: available_modes)
        {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                result = mode;
                break;
            }

            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                result = mode;
                break;
            }
        }

        return result;
    }

    VkExtent2D choose_swap_extent(
            const vk::physical_device_t &device,
            const vk::surface_t &surface,
            const vk::window_t &window)
    {
        auto capabilities = device.surface_capabilities(surface);

        VkExtent2D result {
            capabilities.currentExtent.width,
            capabilities.currentExtent.height
        };

        if (capabilities.currentExtent.width
                == std::numeric_limits<std::uint32_t>::max())
        {
            result.width = std::max(
                               capabilities.minImageExtent.width,
                               std::min(
                                   capabilities.maxImageExtent.width,
                                   window.width()
                                )
                           );
            result.height = std::max(
                                capabilities.minImageExtent.height,
                                std::min(
                                    capabilities.maxImageExtent.height,
                                    window.height()
                                 )
                            );
        }

        return result;
    }
}

void triangle_application::run()
{
    vk::window_t window;
    auto vk_instance = create_instance();
#ifndef NDEBUG
    vk::debug_t d { vk_instance };
#endif
    auto surface = vk::surface_t(vk_instance, window);
    auto physical_device = pick_physical_device(vk_instance, surface);

    const auto queue_graphics_family_index {
        physical_device.queue_family_index(VK_QUEUE_GRAPHICS_BIT)
    };

    const auto queue_family_indices = std::set<int> {
        queue_graphics_family_index,
        physical_device.queue_surface_support_index(surface)
    };

#ifndef NDEBUG
    auto logical_device = vk::logical_device_t(
                physical_device,
                required_device_extensions,
                queue_family_indices,
                validation_layers);
#else
    auto logical_device = vk::logical_device_t(
                physical_device,
                required_device_extensions,
                queue_family_indices);
#endif
    auto graphics_queue = vk::queue_t(logical_device,
                                      queue_graphics_family_index);

    auto swap_chain = vk::swap_chain_t(
                logical_device,
                surface,
                physical_device.surface_capabilities(surface),
                choose_swap_surface_format(physical_device, surface),
                choose_swap_present_mode(physical_device, surface),
                choose_swap_extent(physical_device, surface, window),
                queue_family_indices);

    auto images = swap_chain.images();
    std::vector<std::unique_ptr<vk::image_view_t>> image_views;
    image_views.reserve(images.size());
    for (auto &image: images)
    {
        image_views.emplace_back(new vk::image_view_t(
                                     logical_device,
                                     image,
                                     VK_IMAGE_VIEW_TYPE_2D,
                                     swap_chain.format()));
    }

    auto fragment_shader = vk::shader_t(
                logical_device,
                vk::shader_t::ShaderType::Fragment,
                SHADER_PATH"/frag.spv");
    auto vertex_shader   = vk::shader_t(
                logical_device,
                vk::shader_t::ShaderType::Fragment,
                SHADER_PATH"/vert.spv");

    window.runMainLoop();
}
