#pragma once

#include "voot_global.hh"

#include <spdlog/spdlog.h>

#include <memory>
#include <string_view>

#define VT_LOG_TO_STRING_IMP(x) #x
#define VT_LOG_TO_STRING(x) VT_LOG_TO_STRING_IMP(x)

#define VT_LOG_DEBUG(...)                                                         \
    do                                                                            \
    {                                                                             \
        constexpr auto prefix =                                                   \
            voot::Logger::construct_prefix(__FILE__, VT_LOG_TO_STRING(__LINE__)); \
        voot::Logger::debug(std::string{ prefix.data() } + __VA_ARGS__);          \
    } while (false)
#define VT_LOG_INFO(...)                                                          \
    do                                                                            \
    {                                                                             \
        constexpr auto prefix =                                                   \
            voot::Logger::construct_prefix(__FILE__, VT_LOG_TO_STRING(__LINE__)); \
        voot::Logger::info(std::string{ prefix.data() } + __VA_ARGS__);           \
    } while (false)
#define VT_LOG_WARN(...)                                                          \
    do                                                                            \
    {                                                                             \
        constexpr auto prefix =                                                   \
            voot::Logger::construct_prefix(__FILE__, VT_LOG_TO_STRING(__LINE__)); \
        voot::Logger::warn(std::string{ prefix.data() } + __VA_ARGS__);           \
    } while (false)
#define VT_LOG_ERROR(...)                                                         \
    do                                                                            \
    {                                                                             \
        constexpr auto prefix =                                                   \
            voot::Logger::construct_prefix(__FILE__, VT_LOG_TO_STRING(__LINE__)); \
        voot::Logger::error(std::string{ prefix.data() } + __VA_ARGS__);          \
    } while (false)
#define VT_LOG_FATAL(...)                                                         \
    do                                                                            \
    {                                                                             \
        constexpr auto prefix =                                                   \
            voot::Logger::construct_prefix(__FILE__, VT_LOG_TO_STRING(__LINE__)); \
        voot::Logger::fatal(std::string{ prefix.data() } + __VA_ARGS__);          \
    } while (false);                                                              \
    std::exit(EXIT_FAILURE)

VOOT_BEGIN_NAMESPACE

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

class VOOT_API Logger
{
public:
    static Logger *instance();

public:
    Logger();
    ~Logger() noexcept;

public:
    template<typename... Args> static void debug(std::string_view format, const Args &... args);
    template<typename... Args> static void info(std::string_view format, const Args &... args);
    template<typename... Args> static void warn(std::string_view format, const Args &... args);
    template<typename... Args> static void error(std::string_view format, const Args &... args);
    template<typename... Args> static void fatal(std::string_view format, const Args &... args);

public:
    template<std::size_t file_path_len, std::size_t line_len>
    static constexpr auto construct_prefix(const char (&file_path)[file_path_len],
        const char (&line)[line_len]) noexcept
    {
        const auto file_name_offset = utility::get_file_name_offset(file_path);
        std::array<char, file_path_len + line_len + 1 + 2 + 1> result{};

        std::size_t position = 0;
        for (std::size_t i = 0; i < file_path_len - file_name_offset - 1; ++i, ++position)
        {
            result[position] = file_path[i + file_name_offset];
        }
        result[position++] = ':';
        for (std::size_t i = 0; i < line_len - 1; ++i, ++position)
        {
            result[position] = line[i];
        }
        result[position++] = ':';
        result[position++] = ' ';
        result[position++] = '\0';

        return result;
    }

private:
    std::unique_ptr<Logger> instance_{ nullptr };
    std::unique_ptr<spdlog::logger> logger_{ nullptr };

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
