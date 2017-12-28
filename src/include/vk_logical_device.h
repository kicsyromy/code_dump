#ifndef VK_LOGICAL_DEVICE_H
#define VK_LOGICAL_DEVICE_H

#include "support.h"
#include "vk_physical_device.h"

namespace vk
{
    class logical_device_t
    {
    public:
        template <typename array_t = nullptr_t>
        inline logical_device_t(
                const vk::physical_device_t &vk_physical_device,
                const array_t &validation_layers = nullptr)
          : vk_physical_device_(vk_physical_device)
        {
            VkDeviceQueueCreateInfo queue_create_info { };
            queue_create_info.sType =
                    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex =
                    vk_physical_device.queue_family_index(VK_QUEUE_GRAPHICS_BIT);
            queue_create_info.queueCount = 1;
            float queue_priority { 1.0f };
            queue_create_info.pQueuePriorities = &queue_priority;

            auto features = vk_physical_device.features();

            VkDeviceCreateInfo device_create_info { };
            device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_create_info.pQueueCreateInfos = &queue_create_info;
            device_create_info.queueCreateInfoCount = 1;
            device_create_info.pEnabledFeatures = &features;
            device_create_info.enabledExtensionCount = 0;

            if constexpr (std::is_same_v<array_t, nullptr_t>)
            {
                device_create_info.enabledLayerCount = 0;
            }
            else
            {
                device_create_info.enabledLayerCount = validation_layers.size();
                device_create_info.ppEnabledLayerNames = validation_layers.data();
            }

            if (vkCreateDevice(vk_physical_device, &device_create_info, nullptr, &handle_) != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create logical device");
            }
        }

        inline ~logical_device_t()
        {
            vkDestroyDevice(handle_, nullptr);
        }

    public:
        inline const vk::physical_device_t &physical_device() const
        {
            return vk_physical_device_;
        }

    public:
        inline operator VkDevice() const { return handle_; }

    private:
        VkDevice handle_ { nullptr };
        const vk::physical_device_t &vk_physical_device_;

    private:
        DISABLE_COPY(logical_device_t)
        DISABLE_MOVE(logical_device_t)
    };
}

#endif // !VK_LOGICAL_DEVICE_H
