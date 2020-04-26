#pragma once

VOOT_BEGIN_NAMESPACE

template<typename... Args> void Logger::debug(std::string_view format, const Args &... args)
{
    instance()->logger_->debug(format, args...);
}

template<typename... Args> void Logger::info(std::string_view format, const Args &... args)
{
    instance()->logger_->info(format, args...);
}

template<typename... Args> void Logger::warn(std::string_view format, const Args &... args)
{
    instance()->logger_->warn(format, args...);
}

template<typename... Args> void Logger::error(std::string_view format, const Args &... args)
{
    instance()->logger_->error(format, args...);
}

template<typename... Args> void Logger::fatal(std::string_view format, const Args &... args)
{
    instance()->logger_->critical(format, args...);
#ifndef NDEBUG
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__arm__) || defined(__aarch64__)
    __builtin_trap();
#elif (defined(__i386__) || defined(__amd64__)) && (defined(__GNUC__) || defined(__clang__))
    __asm__("int $3");
#else
    int *int3 = (int *)3L;
    *int3 = 3;
#endif
#else
    std::exit(EXIT_FAILURE);
#endif
}

VOOT_END_NAMESPACE
