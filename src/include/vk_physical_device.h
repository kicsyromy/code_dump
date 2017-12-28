#ifndef VK_PHYSICAL_DEVICE_H
#define VK_PHYSICAL_DEVICE_H

#include <vector>

#include <cstdint>

#include "support.h"
#include "logger.h"
#include "vk_instance.h"

namespace vk
{
    class physical_device_t
    {
    public:
        static std::vector<physical_device_t> physical_devices(
                vk::instance_t &vk_instance)
        {
            std::uint32_t device_count { 0 };
            vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

            std::vector<VkPhysicalDevice> result(device_count);

            if (device_count != 0)
            {
                vkEnumeratePhysicalDevices(
                            vk_instance, &device_count, result.data());
            }

            return { result.begin(), result.end() };
        }

    public:
        inline physical_device_t(VkPhysicalDevice &device_handle)
          : handle_(device_handle) {}
        inline ~physical_device_t() = default;

    public:
        inline VkPhysicalDeviceProperties properties() const
        {
            VkPhysicalDeviceProperties result { };
            vkGetPhysicalDeviceProperties(handle_, &result);
            return result;
        }

        inline VkPhysicalDeviceFeatures   features() const
        {
            VkPhysicalDeviceFeatures result { };
            vkGetPhysicalDeviceFeatures(handle_, &result);
            return result;
        }

        inline std::vector<VkQueueFamilyProperties> queue_families() const
        {
            std::uint32_t count { 0 };
            vkGetPhysicalDeviceQueueFamilyProperties(handle_, &count, nullptr);

            std::vector<VkQueueFamilyProperties> result(count);
            vkGetPhysicalDeviceQueueFamilyProperties(
                        handle_, &count, result.data());

            return result;
        }

        inline int queue_family_index(VkQueueFlags flags) const
        {
            int result { -1 };
            int index  {  0 };

            for (const auto &queue_family: queue_families())
            {
                if ((queue_family.queueCount > 0)
                        && (queue_family.queueFlags & flags))
                {
                    result = index;
                    break;
                }

                ++index;
            }

            return result;
        }

    public:
        inline operator VkPhysicalDevice() const { return handle_; }

    private:
        VkPhysicalDevice handle_ { nullptr };

    public:
        DEFAULT_MOVE(physical_device_t)

    private:
        DISABLE_COPY(physical_device_t)
    };
}

#endif // !VK_PHYSICAL_DEVICE_H
