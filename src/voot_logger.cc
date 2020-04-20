#include "voot_logger.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <cassert>

namespace
{
    std::weak_ptr<voot::Logger> g_logger_instance;
}

VOOT_BEGIN_NAMESPACE

Logger &Logger::instance()
{
    auto logger_instance = g_logger_instance.lock();
    assert(logger_instance != nullptr);
    return *logger_instance;
}

Logger::Logger()
{
    auto logger_instance = g_logger_instance.lock();
    assert(logger_instance == nullptr);

    auto instance = malloc(sizeof(Logger));
    std::memset(instance, 0, sizeof(Logger));

    instance_.reset(static_cast<Logger *>(instance));
    instance_->logger_ = spdlog::stdout_color_mt("voot");

    auto logger = instance_.get()->logger_.get();
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("%^[%T] (%n) %l%$: %v");

    g_logger_instance = instance_;
}

VOOT_END_NAMESPACE
