#ifndef VK_PIPELINE_LAYOUT_H
#define VK_PIPELINE_LAYOUT_H

#include "support.h"
#include "vk_logical_device.h"
#include "logger.h"

namespace vk
{
    class pipeline_layout_t
    {
    public:
        inline pipeline_layout_t(const vk::logical_device_t &device)
          : vk_logical_device_(device)
        {
            VkPipelineLayoutCreateInfo create_info {
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                nullptr,
                0,
                0,
                nullptr,
                0,
                nullptr
            };

            if (vkCreatePipelineLayout(device, &create_info, nullptr, &handle_)
                    != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create pipeline layout");
            }
        }

        inline ~pipeline_layout_t()
        {
            vkDestroyPipelineLayout(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline operator VkPipelineLayout() const { return handle_; }

    private:
        VkPipelineLayout handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(pipeline_layout_t)
        DISABLE_MOVE(pipeline_layout_t)
    };
}

#endif // !VK_PIPELINE_LAYOUT_H
