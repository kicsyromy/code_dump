#ifndef VK_IMAGE_VIEW_H
#define VK_IMAGE_VIEW_H

#include "support.h"
#include "vk_logical_device.h"
#include "logger.h"

namespace vk
{
    class image_view_t
    {
    public:
        inline image_view_t(
                const vk::logical_device_t &logical_device,
                VkImage image,
                VkImageViewType type,
                VkFormat format)
          : vk_logical_device_(logical_device)
        {
            VkImageViewCreateInfo create_info {
                VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                nullptr,
                0,
                image,
                type,
                format,
                {
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY
                },
                {
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    0, 1, 0, 1
                }
            };

            if (vkCreateImageView(
                        logical_device, &create_info, nullptr, &handle_) !=
                    VK_SUCCESS)
            {
                LOG_FATAL("Failed to create image view");
            }
        }

        inline ~image_view_t()
        {
            vkDestroyImageView(vk_logical_device_, handle_, nullptr);
        }

    private:
        VkImageView handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(image_view_t)
        DISABLE_MOVE(image_view_t)
    };
}

#endif // !VK_IMAGE_VIEW_H
