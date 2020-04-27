#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

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
