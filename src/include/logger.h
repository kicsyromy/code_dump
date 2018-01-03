#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstring>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

static const int redirect_io_to_win_console = []() -> int {
    static constexpr WORD MAX_CONSOLE_LINES = 500;

    AllocConsole();

    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    auto win_output_handle =
        reinterpret_cast<std::intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
    auto file_output_handle = _open_osfhandle(win_output_handle, _O_TEXT);
    auto file = _fdopen(file_output_handle, "w");
    *stdout = *file;
    setvbuf(stdout, nullptr, _IONBF, 0);

    win_output_handle =
        reinterpret_cast<std::intptr_t>(GetStdHandle(STD_INPUT_HANDLE));
    file_output_handle = _open_osfhandle(win_output_handle, _O_TEXT);
    file = _fdopen(file_output_handle, "r");
    *stdin = *file;
    setvbuf(stdin, nullptr, _IONBF, 0);

    win_output_handle =
        reinterpret_cast<std::intptr_t>(GetStdHandle(STD_ERROR_HANDLE));
    file_output_handle = _open_osfhandle(win_output_handle, _O_TEXT);
    file = _fdopen(file_output_handle, "w");
    *stderr = *file;
    setvbuf(stderr, nullptr, _IONBF, 0);

    return 1;
}();
#endif

constexpr const std::size_t BUFFER_SIZE { 1024 };
inline thread_local char logbuffer[BUFFER_SIZE + 1];

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

#ifndef NDEBUG
#define LOG_DEBUG(...)                                                        \
    do                                                                        \
    {                                                                         \
        snprintf(logbuffer, BUFFER_SIZE, __VA_ARGS__);                        \
        fprintf(stderr, "DEBUG: %s: %d: %s:\n%s\n",                           \
                   strrchr(__FILE__, PATH_SEPARATOR) + 1, __LINE__, __func__, \
                   logbuffer);                                                \
    } while (0)
#else
#define LOG_DEBUG(...)
#endif // GEARBOX_LOG_DEBUG

#endif // LOGGER_H
