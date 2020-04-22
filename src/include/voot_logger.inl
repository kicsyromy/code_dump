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
}

VOOT_END_NAMESPACE
