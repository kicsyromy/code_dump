#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "core/voot_property.hh"

class TestClass
{
public:
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
    VT_PROPERTY(int, property, &TestClass::get_value, &TestClass::set_value);
    VT_READONLY_PROPERTY(int, read_only_property, &TestClass::get_value);

private:
    int value_;
};

TEST_CASE("Property get and set", "[property]")
{
    TestClass test_class;

    test_class.property = 5;
    REQUIRE(test_class.value_ == 5);

    int a = test_class.property();
    REQUIRE(a == test_class.value_);
}

TEST_CASE("Property signal fire", "[property]")
{
    TestClass test_class;
    static bool event_fired = false;
    test_class.property.changed.connect([](int v) {
        REQUIRE(v == 5);
        event_fired = true;
    });

    test_class.property = 5;
    REQUIRE(event_fired == true);
}
