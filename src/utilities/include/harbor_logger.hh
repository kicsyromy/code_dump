#ifndef HARBOR_LOGGER_HH
#define HARBOR_LOGGER_HH

#ifndef HARBOR_LOGGER_COMPONENT
#define HARBOR_LOGGER_COMPONENT "main"
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

constexpr const std::size_t LOG_BUFFER_SIZE{ 2048 };
inline thread_local char LOG_BUFFER[LOG_BUFFER_SIZE];

constexpr const char FORMAT_STRING_INFO[]{
    "[%d-%d-%d %d:%d:%d] harbor: %s:    INFO: %s: %d: ** %s **:\n%s\n"
};
constexpr const char FORMAT_STRING_WARN[]{
    "[%d-%d-%d %d:%d:%d] harbor: %s: WARNING: %s: %d: ** %s **:\n%s\n"
};
constexpr const char FORMAT_STRING_ERROR[]{
    "[%d-%d-%d %d:%d:%d] harbor: %s:   ERROR: %s: %d: ** %s **:\n%s\n"
};
constexpr const char FORMAT_STRING_FATAL[]{
    "[%d-%d-%d %d:%d:%d] harbor: %s:   FATAL: %s: %d: ** %s **:\n%s\n"
};
constexpr const char FORMAT_STRING_DEBUG[]{
    "[%d-%d-%d %d:%d:%d] harbor: %s:   DEBUG: %s: %d: ** %s **:\n%s\n"
};

#ifdef _MSC_VER
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif // _MSC_VER

#define HARBOR_GET_CURRENT_TIME()                                                                  \
    auto time_point = std::time(nullptr);                                                          \
    const auto local_time = std::localtime(&time_point);                                           \
    const auto year = 1990 + local_time->tm_year;                                                  \
    const auto month = 1 + local_time->tm_mon;                                                     \
    const auto day = local_time->tm_mday;                                                          \
    const auto hour = local_time->tm_hour;                                                         \
    const auto minute = local_time->tm_min;                                                        \
    const auto second = local_time->tm_sec;

#define LOG(...)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        HARBOR_GET_CURRENT_TIME()                                                                  \
        std::snprintf(LOG_BUFFER, LOG_BUFFER_SIZE, __VA_ARGS__);                                   \
        std::fprintf(stdout, FORMAT_STRING_INFO, year, month, day, hour, minute, second,           \
                     HARBOR_LOGGER_COMPONENT, std::strrchr(__FILE__, PATH_SEPARATOR) + 1,          \
                     __LINE__, __func__, LOG_BUFFER);                                              \
    } while (0)

#define LOG_INFO(...) LOG(__VA_ARGS__)

#define LOG_WARN(...)                                                                              \
    do                                                                                             \
    {                                                                                              \
        HARBOR_GET_CURRENT_TIME()                                                                  \
        std::snprintf(LOG_BUFFER, LOG_BUFFER_SIZE, __VA_ARGS__);                                   \
        std::fprintf(stderr, FORMAT_STRING_WARN, year, month, day, hour, minute, second,           \
                     HARBOR_LOGGER_COMPONENT, std::strrchr(__FILE__, PATH_SEPARATOR) + 1,          \
                     __LINE__, __func__, LOG_BUFFER);                                              \
    } while (0)

#define LOG_ERROR(...)                                                                             \
    do                                                                                             \
    {                                                                                              \
        HARBOR_GET_CURRENT_TIME()                                                                  \
        std::snprintf(LOG_BUFFER, LOG_BUFFER_SIZE, __VA_ARGS__);                                   \
        std::fprintf(stderr, FORMAT_STRING_ERROR, year, month, day, hour, minute, second,          \
                     HARBOR_LOGGER_COMPONENT, std::strrchr(__FILE__, PATH_SEPARATOR) + 1,          \
                     __LINE__, __func__, LOG_BUFFER);                                              \
    } while (0)

#define LOG_FATAL(...)                                                                             \
    do                                                                                             \
    {                                                                                              \
        HARBOR_GET_CURRENT_TIME()                                                                  \
        std::snprintf(LOG_BUFFER, LOG_BUFFER_SIZE, __VA_ARGS__);                                   \
        std::fprintf(stderr, FORMAT_STRING_FATAL, year, month, day, hour, minute, second,          \
                     HARBOR_LOGGER_COMPONENT, std::strrchr(__FILE__, PATH_SEPARATOR) + 1,          \
                     __LINE__, __func__, LOG_BUFFER);                                              \
        std::exit(EXIT_FAILURE);                                                                   \
    } while (0)

#ifndef _NDEBUG
#define LOG_DEBUG(...)                                                                             \
    do                                                                                             \
    {                                                                                              \
        HARBOR_GET_CURRENT_TIME()                                                                  \
        std::snprintf(LOG_BUFFER, LOG_BUFFER_SIZE, __VA_ARGS__);                                   \
        std::fprintf(stderr, FORMAT_STRING_DEBUG, year, month, day, hour, minute, second,          \
                     HARBOR_LOGGER_COMPONENT, std::strrchr(__FILE__, PATH_SEPARATOR) + 1,          \
                     __LINE__, __func__, LOG_BUFFER);                                              \
    } while (0)
#else
#define LOG_DEBUG(...)
#endif /* !_NDEBUG */

#endif /* !HARBOR_LOGGER_HH */
