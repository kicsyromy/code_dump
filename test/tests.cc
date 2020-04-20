#include <catch2/catch.hpp>

#include "voot.hh"

TEST_CASE("Logger creation", "[logger]")
{
    voot::Logger logger;
    VT_LOG_WARN("This is a warning");
}
