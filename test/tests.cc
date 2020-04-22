#include <catch2/catch.hpp>

#include "voot.hh"
#include "events/voot_event.hh"

TEST_CASE("Logger creation", "[logger]")
{
    voot::Logger logger;
    VT_LOG_WARN("This is a warning");
    voot::TestEvent event;
    VT_LOG_WARN(event.event_name());
}

TEST_CASE("Event creation", "[event]")
{}
