#ifndef VK_SEMAPHORE_H
#define VK_SEMAPHORE_H

#include "support.h"
#include "vk_logical_device.h"
#include "logger.h"

namespace vk
{
    class semaphore_t
    {
    public:
        inline semaphore_t(const vk::logical_device_t &device)
          : vk_logical_device_(device)
        {
            using namespace vk::support;

            const VkSemaphoreCreateInfo create_info {
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                nullptr,
                0
            };

            if (auto result =
                    vkCreateSemaphore(device, &create_info, nullptr, &handle_);
                    result != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create semaphore. Error: %s",
                          VK_RESULT_STRING.at(result));
            }
        }

        inline ~semaphore_t()
        {
            vkDestroySemaphore(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline operator VkSemaphore() const { return handle_; }

    private:
        VkSemaphore handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(semaphore_t)
        DISABLE_MOVE(semaphore_t)
    };
}

#endif // !VK_SEMAPHORE_H
