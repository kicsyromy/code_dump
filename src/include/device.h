#ifndef VK_TUTORIAL_DEVICE_H
#define VK_TUTORIAL_DEVICE_H

#include <vector>
#include <array>

#include <cstdint>

#include <vulkan.h>

#include "global.h"
#include "queue.h"

namespace vulkan_tutorial
{
    class physical_device_t;

    template<std::size_t count>
    struct device_queue_t
    {
        std::uint32_t family_index;
        std::array<float, count> priorities;
    };

    class device_t
    {
    public:
        template<
            typename device_queue_array_t,
            typename layer_array_t,
            typename extension_array_t
        >
        device_t(
                const physical_device_t &physical_device,
                const device_queue_array_t &device_queues,
                const layer_array_t &enabled_layers,
                const extension_array_t &enabled_extensions,
                const VkPhysicalDeviceFeatures &desired_features) noexcept;
        ~device_t() noexcept;

    public:
        DEFAULT_MOVE(device_t)

    public:
        inline const std::vector<std::string_view> &
        enabled_layers() const noexcept { return enabled_layers_; }

        inline const std::vector<std::string_view> &
        enabled_extensions() const noexcept { return enabled_extensions_; }

        queue_t queue(
                std::uint32_t family_index, std::uint32_t index) const noexcept;

        inline bool valid() const noexcept { return handle_ != nullptr; }

    public:
        inline operator VkDevice() noexcept { return handle_; }
        inline operator const VkDevice() const noexcept { return handle_; }

    private:
        void initialize_device_handle(
                const physical_device_t &physical_device,
                const VkDeviceCreateInfo &create_info) noexcept;

    private:
        VkDevice handle_ { nullptr };
        const std::vector<std::string_view> enabled_layers_;
        const std::vector<std::string_view> enabled_extensions_;

    private:
        DISABLE_COPY(device_t)
    };

    template<
        typename device_queue_array_t,
        typename layer_array_t,
        typename extension_array_t
    >
    device_t::device_t(
            const physical_device_t &physical_device,
            const device_queue_array_t &device_queues,
            const layer_array_t &enabled_layers,
            const extension_array_t &enabled_extensions,
            const VkPhysicalDeviceFeatures &desired_features) noexcept
      : enabled_layers_ { enabled_layers.cbegin(), enabled_layers.cend() }
      , enabled_extensions_ {
            enabled_extensions.cbegin(), enabled_extensions.cend()
        }
    {
        constexpr auto count = device_queues.size();
        std::array<VkDeviceQueueCreateInfo, count> device_queue_create_infos;

        for (std::size_t index = 0; index < count; ++index)
        {
            device_queue_create_infos[index] = VkDeviceQueueCreateInfo {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,
                device_queues[index].family_index,
                1,
                device_queues[index].priorities.data()
            };
        }

        const VkDeviceCreateInfo device_create_info {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            0,
            count,
            device_queue_create_infos.data(),
            static_cast<std::uint32_t>(enabled_layers.size()),
            enabled_layers.data(),
            static_cast<std::uint32_t>(enabled_extensions.size()),
            enabled_extensions.data(),
            &desired_features
        };

        initialize_device_handle(physical_device, device_create_info);
    }
}

#endif // !VK_TUTORIAL_DEVICE_H
