#ifndef VK_SWAP_CHAIN_H
#define VK_SWAP_CHAIN_H

#include <set>

#include "support.h"
#include "vk_physical_device.h"
#include "vk_logical_device.h"
#include "vk_surface.h"
#include "vk_window.h"
#include "logger.h"

namespace vk
{
    class swap_chain_t
    {
    public:
        inline swap_chain_t(
                const vk::logical_device_t &logical_device,
                const vk::surface_t &surface,
                VkSurfaceCapabilitiesKHR capabilities,
                VkSurfaceFormatKHR surface_format,
                VkPresentModeKHR present_mode,
                VkExtent2D extent,
                const std::set<int> &queue_family_indices)
          : vk_logical_device_(logical_device)
          , format_(surface_format)
          , extent_(extent)
        {
            std::uint32_t image_count = capabilities.minImageCount + 1;
            if (capabilities.maxImageCount > 0 &&
                image_count > capabilities.maxImageCount)
            {
                image_count = capabilities.maxImageCount;
            }

            std::vector<std::uint32_t> queue_indices {
                queue_family_indices.begin(), queue_family_indices.end()
            };

            VkSwapchainCreateInfoKHR create_info { };
            create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            create_info.surface = surface;
            create_info.minImageCount = image_count;
            create_info.imageFormat = surface_format.format;
            create_info.imageColorSpace = surface_format.colorSpace;
            create_info.imageExtent = extent;
            create_info.imageArrayLayers = 1;
            create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if (queue_indices.size() > 1)
            {
                create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                create_info.queueFamilyIndexCount =
                        static_cast<std::uint32_t>(queue_indices.size());
                create_info.pQueueFamilyIndices = queue_indices.data();
            }
            else
            {
                create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                create_info.queueFamilyIndexCount = 0;
                create_info.pQueueFamilyIndices = nullptr;
            }

            create_info.preTransform = capabilities.currentTransform;
            create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            create_info.presentMode = present_mode;
            create_info.clipped = VK_TRUE;
            create_info.oldSwapchain = nullptr;

            if (vkCreateSwapchainKHR(
                        logical_device, &create_info, nullptr, &handle_)
                    != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create swap chain");
            }
        }

        inline ~swap_chain_t()
        {
            vkDestroySwapchainKHR(vk_logical_device_, handle_, nullptr);
        }

    public:
        VkFormat format() const { return format_.format; }
        VkColorSpaceKHR color_space() const { return format_.colorSpace; }
        VkExtent2D extent() const { return extent_; }

    public:
        inline std::vector<VkImage> images() const
        {
            std::uint32_t count { 0 };
            vkGetSwapchainImagesKHR(
                        vk_logical_device_, handle_, &count, nullptr);

            std::vector<VkImage> result(count);
            vkGetSwapchainImagesKHR(
                        vk_logical_device_, handle_, &count, result.data());

            return result;
        }

    public:
        inline operator VkSwapchainKHR() const { return handle_; }

    private:
        VkSwapchainKHR handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;
        VkSurfaceFormatKHR format_ {
            static_cast<VkFormat>(0),
            static_cast<VkColorSpaceKHR>(0)
        };
        VkExtent2D extent_ { 0, 0 };

    private:
        DISABLE_COPY(swap_chain_t)
        DISABLE_MOVE(swap_chain_t)
    };
}

#endif // !VK_SWAP_CHAIN_H
