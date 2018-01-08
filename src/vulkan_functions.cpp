#include <vulkan.h>

#if defined(_WIN32)
#include <windows.h>
HMODULE load_vulkan_libary() { return LoadLibrary("vulkan-1.dll"); }
#define LOAD_FUNCTION GetProcAddress
#elif defined(__linux)
#include <dlfcn.h>
void *load_vulkan_libary() { return dlopen("libvulkan.so.1", RTLD_NOW); }
#define LOAD_FUNCTION dlsym
#endif

#include "vulkan_instance.h"
#include "device.h"
#include "logger.h"

namespace vulkan_tutorial
{
    static auto vulkan_library = [] {
        auto handle = load_vulkan_libary();

        if (handle == nullptr)
        {
            LOG_FATAL("Failed to connect to Vulkan Runtime Library.");
        }

        return handle;
    }();

    #define EXPORTED_VULKAN_FUNCTION(name) \
        PFN_##name name { nullptr };

    #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) \
        PFN_##name name { nullptr };

    #define INSTANCE_LEVEL_VULKAN_FUNCTION(name) \
        PFN_##name name { nullptr };

    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
        PFN_##name name { nullptr };

    #define DEVICE_LEVEL_VULKAN_FUNCTION(name) \
        PFN_##name name { nullptr };

    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
        PFN_##name name { nullptr };

    #include "vulkan_functions.inl"

    #undef EXPORTED_VULKAN_FUNCTION
    #undef GLOBAL_LEVEL_VULKAN_FUNCTION
    #undef INSTANCE_LEVEL_VULKAN_FUNCTION
    #undef INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
    #undef DEVICE_LEVEL_VULKAN_FUNCTION
    #undef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION

    bool load_exported_functions()
    {
        auto result { true };

        #define EXPORTED_VULKAN_FUNCTION(name) \
            name = [&result]() noexcept {                                   \
                auto function = reinterpret_cast<PFN_##name>(               \
                            LOAD_FUNCTION(vulkan_library, #name));          \
                if (result = result && function != nullptr; !result)        \
                {                                                           \
                    LOG_ERROR("Failed to load exported function "#name);    \
                }                                                           \
                return function;                                            \
            }();

        #include "vulkan_functions.inl"

        #undef EXPORTED_VULKAN_FUNCTION

        return result;
    }

    bool load_global_functions()
    {
        auto result { true };

        #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) \
             name = [&result]() noexcept {                                 \
                auto function = reinterpret_cast<PFN_##name>(              \
                            vkGetInstanceProcAddr(nullptr, #name));        \
                if (result = result && function != nullptr; !result)       \
                {                                                          \
                    LOG_ERROR("Failed to load global function "#name);     \
                }                                                          \
                return function;                                           \
            }();

        #include "vulkan_functions.inl"

        #undef GLOBAL_LEVEL_VULKAN_FUNCTION

        return result;
    }

    bool load_instance_level_functions(
            const vulkan_instance_t &instance)
    {
        auto result { true };

        #define INSTANCE_LEVEL_VULKAN_FUNCTION(name) \
             name = [&result, &instance]() noexcept {                      \
                auto function = reinterpret_cast<PFN_##name>(              \
                            vkGetInstanceProcAddr(instance, #name));       \
                if (result = result && function != nullptr; !result)       \
                {                                                          \
                    LOG_ERROR("Failed to load instance function "#name);   \
                }                                                          \
                return function;                                           \
            }();

        #include "vulkan_functions.inl"

        #undef INSTANCE_LEVEL_VULKAN_FUNCTION

        return result;
    }

    bool load_instance_level_functions_from_extensions(
            const vulkan_instance_t &instance)
    {
        auto result { true };

        #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
             name = [&result, &instance]() noexcept {                      \
                auto function = reinterpret_cast<PFN_##name>(              \
                            vkGetInstanceProcAddr(instance, #name));       \
                if (result = result && function != nullptr; !result)       \
                {                                                          \
                    LOG_ERROR("Failed to load instance function "#name     \
                              " from extension "#extension);               \
                }                                                          \
                return function;                                           \
            }();

        #include "vulkan_functions.inl"

        #undef INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION

        return result;
    }

    bool load_device_level_functions(const device_t &device)
    {
        auto result { true };

        #define DEVICE_LEVEL_VULKAN_FUNCTION(name) \
             name = [&result, &device]() noexcept {                        \
                auto function = reinterpret_cast<PFN_##name>(              \
                            vkGetDeviceProcAddr(device, #name));           \
                if (result = result && function != nullptr; !result)       \
                {                                                          \
                    LOG_ERROR("Failed to load device function "#name);     \
                }                                                          \
                return function;                                           \
            }();

        #include "vulkan_functions.inl"

        #undef DEVICE_LEVEL_VULKAN_FUNCTION

        return result;
    }

    bool load_device_level_functions_from_extensions(
            const device_t &device)
    {
        auto result { true };

        #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
             name = [&result, &device]() noexcept {                        \
                auto function = reinterpret_cast<PFN_##name>(              \
                            vkGetDeviceProcAddr(device, #name));           \
                if (result = result && function != nullptr; !result)       \
                {                                                          \
                    LOG_ERROR("Failed to load device function "#name       \
                              " from extension "#extension);               \
                }                                                          \
                return function;                                           \
            }();

        #include "vulkan_functions.inl"

        #undef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION

        return result;
    }
}
