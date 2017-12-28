#ifndef VK_QUEUE_H
#define VK_QUEUE_H

#include "support.h"
#include "vk_logical_device.h"

namespace vk
{
    class queue_t
    {
    public:
        inline queue_t(const vk::logical_device_t &vk_logical_device)
        {
            vkGetDeviceQueue(
                vk_logical_device,
                vk_logical_device.physical_device()
                    .queue_family_index(VK_QUEUE_GRAPHICS_BIT),
                0,
                &handle_
            );
        }

        inline ~queue_t() = default;

    private:
        VkQueue handle_ { nullptr };

    private:
        DISABLE_COPY(queue_t)
        DISABLE_MOVE(queue_t)
    };
}

#endif // !VK_QUEUE_H
