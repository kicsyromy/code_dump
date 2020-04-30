#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "core/voot_binding.hh"
#include "voot_binding.cc"

struct S1
{
    int get_value() const noexcept
    {
        return value_;
    }

    bool set_value(int value) noexcept
    {
        if (value_ != value)
        {
            value_ = value;
            return true;
        }

        return false;
    }

public:
    VT_PROPERTY(int, property, &S1::get_value, &S1::set_value);

private:
    int value_;
};

struct S2
{
    int get_value() const noexcept
    {
        return value_;
    }

    bool set_value(int value) noexcept
    {
        if (value_ != value)
        {
            value_ = value;
            return true;
        }

        return false;
    }

public:
    VT_PROPERTY(int, property, &S2::get_value, &S2::set_value);

private:
    int value_;
};

TEST_CASE("Bind 2 properties", "[binding]")
{
    using namespace voot;

    S1 s1;
    S2 s2;

    voot::bind(s1.property, s2.property);
    s2.property = 7;

    REQUIRE(s1.value_ == 7);
    REQUIRE(s1.property() == 7);
}

TEST_CASE("Bind property to expression", "[binding]")
{
    using namespace voot;

    S1 s1;
    bool binding_called = false;

    voot::bind(s1.property, [&binding_called] {
        binding_called = true;
        return 12;
    });

    REQUIRE(s1.property() == 12);
    REQUIRE(binding_called == true);
}
