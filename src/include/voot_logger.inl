#pragma once

VOOT_BEGIN_NAMESPACE

template<typename... Args> void Logger::debug(std::string_view format, const Args &... args)
{
    instance().logger_.get()->debug(format, args...);
}

template<typename... Args> void Logger::info(std::string_view format, const Args &... args)
{
    instance().logger_.get()->info(format, args...);
}

template<typename... Args> void Logger::warn(std::string_view format, const Args &... args)
{
    instance().logger_.get()->warn(format, args...);
}

template<typename... Args> void Logger::error(std::string_view format, const Args &... args)
{
    instance().logger_.get()->error(format, args...);
}

template<typename... Args> void Logger::fatal(std::string_view format, const Args &... args)
{
    instance().logger_.get()->critical(format, args...);
}

VOOT_END_NAMESPACE
