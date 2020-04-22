#pragma once

#include "voot_global.hh"

#include <spdlog/spdlog.h>

#include <memory>
#include <string_view>

#define VT_LOG_DEBUG(...) voot::Logger::debug(__VA_ARGS__)
#define VT_LOG_INFO(...) voot::Logger::info(__VA_ARGS__)
#define VT_LOG_WARN(...) voot::Logger::warn(__VA_ARGS__)
#define VT_LOG_ERROR(...) voot::Logger::error(__VA_ARGS__)
#define VT_LOG_FATAL(...) voot::Logger::fatal(__VA_ARGS__)

VOOT_BEGIN_NAMESPACE

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

class VOOT_API Logger
{
public:
    static std::shared_ptr<Logger> instance();

public:
    Logger();

public:
    template<typename... Args> static void debug(std::string_view format, const Args &... args);
    template<typename... Args> static void info(std::string_view format, const Args &... args);
    template<typename... Args> static void warn(std::string_view format, const Args &... args);
    template<typename... Args> static void error(std::string_view format, const Args &... args);
    template<typename... Args> static void fatal(std::string_view format, const Args &... args);

private:
    std::shared_ptr<Logger> instance_{ nullptr };
    std::shared_ptr<spdlog::logger> logger_{ nullptr };

private:
    Logger(void *) noexcept {};
    VOOT_DISABLE_COPY(Logger);
    VOOT_DISABLE_MOVE(Logger);
};

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

VOOT_END_NAMESPACE

#include "voot_logger.inl"
