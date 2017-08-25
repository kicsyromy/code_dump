#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstring>

constexpr const std::size_t BUFFER_SIZE { 1024 };
extern thread_local char logbuffer[BUFFER_SIZE + 1] = { 0 };

#ifdef _MSC_VER
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif // _MSC_VER

#define LOG(...)                                                           \
    do                                                                     \
    {                                                                      \
        snprintf(logbuffer, BUFFER_SIZE, __VA_ARGS__);                     \
        fprintf(stderr, "INFO: %s: %d: %s:\n%s\n",                         \
                strrchr(__FILE__, PATH_SEPARATOR) + 1, __LINE__, __func__, \
                logbuffer);                                                \
    } while (0)

#define LOG_INFO(...) LOG(__VA_ARGS__)

#define LOG_WARN(...)                                                       \
    do                                                                      \
    {                                                                       \
        snprintf(logbuffer, BUFFER_SIZE, __VA_ARGS__);                      \
        fprintf(stderr, "WARNING: %s: %d: %s:\n%s\n",                       \
                strrchr(__FILE__, PATH_SEPARATOR) + 1, __LINE__, __func__,  \
                logbuffer);                                                 \
    } while (0)

#define LOG_ERROR(...)                                                      \
    do                                                                      \
    {                                                                       \
        snprintf(logbuffer, BUFFER_SIZE, __VA_ARGS__);                      \
        fprintf(stderr, "ERROR: %s: %d: %s:\n%s\n",                         \
                strrchr(__FILE__, PATH_SEPARATOR) + 1, __LINE__, __func__,  \
                logbuffer);                                                 \
    } while (0)

#define LOG_FATAL(...)                                                      \
    do                                                                      \
    {                                                                       \
        snprintf(logbuffer, BUFFER_SIZE, __VA_ARGS__);                      \
        fprintf(stderr, "FATAL: %s: %d: %s:\n%s\n",                         \
                strrchr(__FILE__, PATH_SEPARATOR) + 1, __LINE__, __func__,  \
                logbuffer);                                                 \
        std::terminate();                                                   \
    } while (0)

#ifdef _DEBUG
#define LOG_DEBUG(...)                                                         \
    do                                                                         \
    {                                                                          \
        snprintf(logbuffer, BUFFER_SIZE, __VA_ARGS__);                         \
        fprintf(stderr, "DEBUG: %s: %d: %s:\n%s\n",                            \
                   strrchr(__FILE__, PATH_SEPARATOR) + 1, __LINE__, __func__,  \
                   logbuffer);                                                 \
    } while (0)
#else
#define LOG_DEBUG(...)
#endif // GEARBOX_LOG_DEBUG

#endif // LOGGER_H
