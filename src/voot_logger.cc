#include "core/voot_logger.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <cassert>

namespace
{
    voot::Logger *g_logger_instance;
}

VOOT_BEGIN_NAMESPACE

Logger *Logger::instance()
{
    return g_logger_instance;
}

Logger::Logger()
  : instance_{ new Logger{ nullptr } }
{
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto logger = std::make_unique<spdlog::logger>("voot", stdout_sink);
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("%^[%T] (%n) %l%$ %v");

    instance_->logger_ = std::move(logger);

    g_logger_instance = instance_.get();
}

voot::Logger::~Logger() noexcept
{
    g_logger_instance = nullptr;
}

VOOT_END_NAMESPACE
