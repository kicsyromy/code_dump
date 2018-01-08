#ifndef VULKAN_FUNCTIONS_H
#define VULKAN_FUNCTIONS_H

#include <vulkan.h>

#include "vulkan_instance.h"
#include "device.h"

namespace vulkan_tutorial
{
    #define EXPORTED_VULKAN_FUNCTION(name) \
        extern PFN_##name name;

    #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) \
        extern PFN_##name name;

    #define INSTANCE_LEVEL_VULKAN_FUNCTION(name) \
        extern PFN_##name name;

    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
        extern PFN_##name name;

    #define DEVICE_LEVEL_VULKAN_FUNCTION(name) \
        extern PFN_##name name;

    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
        extern PFN_##name name;

    #include "vulkan_functions.inl"

    extern bool load_exported_functions();
    extern bool load_global_functions();
    extern bool load_instance_level_functions(
                    const vulkan_instance_t &instance);
    extern bool load_instance_level_functions_from_extensions(
                    const vulkan_instance_t &instance);
    extern bool load_device_level_functions(const device_t &device);
    extern bool load_device_level_functions_from_extensions(
                    const device_t &device);
}

#endif // !VULKAN_FUNCTIONS_H
