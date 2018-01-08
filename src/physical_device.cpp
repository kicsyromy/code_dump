#include "physical_device.h"

#include <algorithm>

#include <cstdint>

#include "vulkan_functions.h"

using namespace vulkan_tutorial;

std::vector<VkExtensionProperties>
physical_device_t::available_extensions() const noexcept
{
    std::uint32_t count { 0 };
    vkEnumerateDeviceExtensionProperties(handle_, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> result(count);
    vkEnumerateDeviceExtensionProperties(
                handle_, nullptr, &count, result.data());

    return result;
}

bool physical_device_t::extension_available(
        std::string_view desired_extension) const noexcept
{
    static const auto extensions = available_extensions();

    auto it = std::find_if(
        extensions.cbegin(),
        extensions.cend(),
        [&desired_extension](const auto &extension) {
            return (desired_extension == extension.extensionName);
        }
    );

    return (it != extensions.cend());
}

VkPhysicalDeviceFeatures physical_device_t::features() const noexcept
{
    VkPhysicalDeviceFeatures result { };
    vkGetPhysicalDeviceFeatures(handle_, &result);
    return result;
}

VkPhysicalDeviceProperties physical_device_t::properties() const noexcept
{
    VkPhysicalDeviceProperties result { };
    vkGetPhysicalDeviceProperties(handle_, &result);
    return result;
}

std::vector<VkQueueFamilyProperties>
physical_device_t::queue_families() const noexcept
{
    std::uint32_t count { 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(handle_, &count, nullptr);

    std::vector<VkQueueFamilyProperties> result(count);
    vkGetPhysicalDeviceQueueFamilyProperties(handle_, &count, result.data());

    return result;
}

std::optional<std::uint32_t> physical_device_t::queue_family_index(
        VkQueueFlags desired_capabilities) const noexcept
{
    static const auto queue_family_array = queue_families();

    std::optional<std::uint32_t> result { };

    for (std::size_t it = 0; it < queue_family_array.size(); ++it)
    {
        const auto &queue_family_properties = queue_family_array.at(it);
        if ((queue_family_properties.queueCount > 0) &&
                (queue_family_properties.queueFlags & desired_capabilities))
        {
            result = static_cast<std::uint32_t>(it);
            break;
        }
    }

    return result;
}
