#ifndef VK_QUEUE_H
#define VK_QUEUE_H

#include "support.h"
#include "vk_logical_device.h"

namespace vk
{
    class queue_t
    {
    public:
        inline queue_t(
            const vk::logical_device_t &vk_logical_device,
            int queue_family_index)
        {
            vkGetDeviceQueue(
                vk_logical_device,
                static_cast<std::uint32_t>(queue_family_index),
                0,
                &handle_
            );
        }

        inline ~queue_t() = default;

    public:
        void wait_for_idle() const
        {
            vkQueueWaitIdle(handle_);
        }

    public:
        inline operator VkQueue() const { return handle_; }

    private:
        VkQueue handle_ { nullptr };

    private:
        DISABLE_COPY(queue_t)
        DISABLE_MOVE(queue_t)
    };
}

#endif // !VK_QUEUE_H
