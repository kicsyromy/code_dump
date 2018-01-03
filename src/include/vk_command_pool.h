#ifndef VK_COMMAND_POOL_H
#define VK_COMMAND_POOL_H

#include "support.h"
#include "vk_logical_device.h"
#include "logger.h"

namespace vk
{
    class command_pool_t
    {
    public:
        inline command_pool_t(
                const vk::logical_device_t &device,
                std::uint32_t queue_family_index)
          : vk_logical_device_(device)
        {
            using namespace vk::support;

            const VkCommandPoolCreateInfo create_info {
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                nullptr,
                0,
                queue_family_index
            };

            if (auto result = vkCreateCommandPool(
                        device,
                        &create_info,
                        nullptr,
                        &handle_); result != VK_SUCCESS)
            {

                LOG_FATAL("Failed to create command pool. Error: %s",
                          VK_RESULT_STRING.at(result));
            }
        }

        inline ~command_pool_t()
        {
            vkDestroyCommandPool(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline operator VkCommandPool() const { return handle_; }

    private:
        VkCommandPool handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(command_pool_t)
        DISABLE_MOVE(command_pool_t)
    };
}

#endif // !VK_COMMAND_POOL_H
