#include "voot_logger.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <cassert>

namespace
{
    std::weak_ptr<voot::Logger> g_logger_instance;
}

VOOT_BEGIN_NAMESPACE

std::shared_ptr<Logger> Logger::instance()
{
    auto logger_instance = g_logger_instance.lock();
    assert(logger_instance != nullptr);
    return logger_instance;
}

Logger::Logger()
  : instance_{ new Logger{ nullptr } }
{
    auto logger_instance = g_logger_instance.lock();
    assert(logger_instance == nullptr);

    auto logger = spdlog::stdout_color_mt("voot");
    auto l = logger.get();
    l->set_level(spdlog::level::debug);
    l->set_pattern("%^[%T] (%n) %l%$: %v");

    instance_->logger_ = std::move(logger);

    g_logger_instance = instance_;
}

VOOT_END_NAMESPACE
