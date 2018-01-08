#include <vector>
#include <array>

#include "vulkan_functions.h"
#include "vulkan_instance.h"
#include "physical_device.h"
#include "device.h"
#include "logger.h"

namespace
{
    constexpr const auto required_extensions = std::array {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    constexpr const auto required_device_extensions = std::array {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
}

int main()
{
    using namespace vulkan_tutorial;

    load_exported_functions();
    load_global_functions();

    for (const auto &desired_extension: required_extensions)
    {
        auto result = vulkan_instance_t::extension_available(desired_extension);
        if (!result)
        {
            LOG_FATAL("Missing required extension %s", desired_extension);
        }
    }

    auto vulkan_instance = vulkan_instance_t {
        "Vulkan Tutorial",
        1,
        "No engine",
        1,
        { },
        {
            required_extensions.cbegin(),
            required_extensions.cend()
        }
    };

    load_instance_level_functions(vulkan_instance);
    load_instance_level_functions_from_extensions(vulkan_instance);

    auto physical_devices = vulkan_instance.physical_devices();
    for (const auto &device: physical_devices)
    {
        for (const auto &desired_extension: required_device_extensions)
        {
            if (!(device.extension_available(desired_extension)))
            {
                LOG_ERROR("Device %p does not support %s",
                          static_cast<VkPhysicalDevice>(device),
                          desired_extension);
            }
        }
    }

    auto physical_device = std::move(physical_devices[0]);
    auto features = physical_device.features();
    auto properties = physical_device.properties();
    auto queue_families = physical_device.queue_families();

    const auto graphics_queue_family_index =
            physical_device.queue_family_index(VK_QUEUE_GRAPHICS_BIT);

    const auto compute_queue_family_index =
            physical_device.queue_family_index(VK_QUEUE_COMPUTE_BIT);

    if (!graphics_queue_family_index)
    {
        LOG_FATAL("Failed to obtain index of graphics queue");
    }

    if (!compute_queue_family_index)
    {
        LOG_FATAL("Failed to obtain index of compute queue");
    }

    const auto device_queues = std::array {
        device_queue_t<1> { *graphics_queue_family_index, { 1.0f }}
    };

    auto logical_device = device_t(
        physical_device,
        device_queues,
        std::array<const char *, 0> { },
        required_device_extensions,
        { }
    );

    load_device_level_functions(logical_device);
    load_device_level_functions_from_extensions(logical_device);

    return 0;
}
