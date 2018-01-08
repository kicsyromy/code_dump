#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <vector>
#include <string_view>
#include <optional>

#include <vulkan.h>

#include "global.h"

namespace vulkan_tutorial
{
    class physical_device_t
    {
    public:
        physical_device_t() noexcept = default;
        ~physical_device_t() noexcept = default;

    public:
        DEFAULT_MOVE(physical_device_t)

    public:
        std::vector<VkExtensionProperties>
        available_extensions() const noexcept;

        bool extension_available(
                std::string_view desired_extension) const noexcept;
        inline bool valid() const noexcept { return handle_ != nullptr; }

        VkPhysicalDeviceFeatures features() const noexcept;
        VkPhysicalDeviceProperties properties() const noexcept;

        std::vector<VkQueueFamilyProperties> queue_families() const noexcept;
        std::optional<uint32_t> queue_family_index(
                VkQueueFlags desired_capabilities) const noexcept;

    public:
        inline operator VkPhysicalDevice() noexcept { return handle_; }
        inline operator const VkPhysicalDevice() const noexcept
        {
            return handle_;
        }

    private:
        VkPhysicalDevice handle_ { nullptr };

    private:
        DISABLE_COPY(physical_device_t)
    };
}

#endif // !PHYSICAL_DEVICE_H
