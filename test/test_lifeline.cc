#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "core/voot_lifeline.hh"
#include "voot_lifeline.cc"

class WithLifeline
{
    VT_ADD_LIFELINE
};

TEST_CASE("Basic setup", "[lifeline]")
{
    WithLifeline test;

    REQUIRE(test.lifeline_.get() != nullptr);
    REQUIRE(test.lifeline_.get() == &voot::lifeline_data);
}

TEST_CASE("Weakline", "[lifeline]")
{
    WithLifeline test;

    auto weakline = test.get_lifeline();
    auto strongline = voot::get_lifeline(weakline);

    REQUIRE(test.lifeline_ == strongline);
    REQUIRE(strongline.get() == &voot::lifeline_data);
}

TEST_CASE("Weakline after lifeline destruction", "[lifeline]")
{
    voot::Weakline weakline;
    {
        WithLifeline test;
        weakline = test.get_lifeline();
    }
    auto strongline = voot::get_lifeline(weakline);

    REQUIRE(strongline == nullptr);
    REQUIRE(voot::lifeline_data == 0);
}
