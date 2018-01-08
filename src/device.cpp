#include "device.h"

#include "vulkan_functions.h"
#include "vulkan_instance.h"
#include "physical_device.h"
#include "logger.h"

using namespace vulkan_tutorial;

device_t::~device_t() noexcept
{
    if (valid())
    {
        vkDestroyDevice(handle_, nullptr);
    }
}

void device_t::initialize_device_handle(
        const physical_device_t &physical_device,
        const VkDeviceCreateInfo &create_info) noexcept
{
    if (auto r = vkCreateDevice(
                physical_device, &create_info, nullptr, &handle_);
            r != VK_SUCCESS)
    {
        handle_ = nullptr;
        LOG_ERROR("Failed to create logical device from %p. Error: %s",
                  static_cast<const VkPhysicalDevice>(physical_device),
                  vulkan_instance_t::vk_result_to_string(r).data());
    }
}
