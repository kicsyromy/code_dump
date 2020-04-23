#include <catch2/catch.hpp>

#define private public

#include "core/voot_logger.hh"
#include "voot_logger.cc"

TEST_CASE("Logger lifetime", "[logger]")
{
    {
        voot::Logger logger;
        REQUIRE(logger.logger_ == nullptr);
        REQUIRE(logger.instance_ != nullptr);
        REQUIRE(g_logger_instance != nullptr);
    }
    REQUIRE(g_logger_instance == nullptr);
}

TEST_CASE("Logger destroyed and recreated properly", "[logger]")
{
    voot::Logger l;
}
