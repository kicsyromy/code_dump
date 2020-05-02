#pragma once

#include <cstring>
#include <cstdint>
#include <string>

#ifndef VOOT_TESTING
#if defined(_MSC_VER)
#define VOOT_EXPORT_SYMBOL __declspec(dllexport)
#define VOOT_IMPORT_SYMBOL __declspec(dllimport)
#elif defined(__GNUC__)
#define VOOT_EXPORT_SYMBOL __attribute__((visibility("default")))
#define VOOT_IMPORT_SYMBOL
#else
#define VOOT_EXPORT_SYMBOL
#define VOOT_IMPORT_SYMBOL
#pragma warning Unknown dynamic link import / export semantics.
#endif
#else
#define VOOT_EXPORT_SYMBOL
#define VOOT_IMPORT_SYMBOL
#endif

#ifdef VOOT_LIB
#define VOOT_API VOOT_EXPORT_SYMBOL
#else
#define VOOT_API VOOT_IMPORT_SYMBOL
#endif

#define VOOT_BEGIN_NAMESPACE \
    namespace voot           \
    {
#define VOOT_END_NAMESPACE }

#define VOOT_SINGLETON(klass) klass() = delete;

#define VOOT_DISABLE_COPY(klass)            \
    explicit klass(const klass &) = delete; \
    klass &operator=(const klass &) = delete

#define VOOT_DISABLE_MOVE(klass)       \
    explicit klass(klass &&) = delete; \
    klass &operator=(klass &&) = delete

#define VT_CONSTEXPR_VALUE(exp) voot::utility::constexpr_value<decltype(exp), exp>::value

#ifdef __linux__
extern const std::string VT_WAYLAND_DISPLAY;
#endif

VOOT_BEGIN_NAMESPACE

namespace utility
{
    template<typename T, T v> struct constexpr_value
    {
        static constexpr const T value = v;
    };

    template<std::size_t S>
    constexpr std::size_t get_file_name_offset(const char (&str)[S], size_t i = S - 1) noexcept
    {
        return (str[i] == '/' || str[i] == '\\') ? i + 1
                                                 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
    }

    constexpr std::uint32_t rgba(std::uint8_t red,
        std::uint8_t green,
        std::uint8_t blue,
        std::uint8_t alpha = 0xFF) noexcept
    {
        return std::uint32_t((red << 24) | (green << 16) | (blue << 8) | alpha);
    }

    template<typename...> struct GetClassType : std::false_type
    {
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...)>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) const>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) volatile>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) const volatile>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) const noexcept>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) noexcept>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) volatile noexcept>
    {
        using Type = Class;
    };
    template<typename R, typename Class, typename... Args>
    struct GetClassType<R (Class::*)(Args...) const volatile noexcept>
    {
        using Type = Class;
    };

} // namespace utility

VOOT_END_NAMESPACE
