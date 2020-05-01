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

static bool get_s1_called = false;

struct S1
{
    int get_value() const noexcept
    {
        get_s1_called = true;
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

    get_s1_called = false;

    S1 s1;
    S2 s2;

    static bool s1_change_triggered = false;
    s1.property.changed.connect([](int value) {
        s1_change_triggered = true;
        REQUIRE(value == 7);
    });

    voot::bind(s1.property, s2.property);
    s2.property = 7;

    REQUIRE(s1_change_triggered == true);
    REQUIRE(get_s1_called == false);
    REQUIRE(s1.property() == 7);
}

TEST_CASE("Bind property to expression", "[binding]")
{
    using namespace voot;

    get_s1_called = false;

    S1 s1;
    bool binding_called = false;

    voot::bind(s1.property, [&binding_called] {
        binding_called = true;
        return 12;
    });

    REQUIRE(get_s1_called == false);
    REQUIRE(s1.property() == 12);
    REQUIRE(binding_called == true);
}

TEST_CASE("Property const &", "[binding]")
{
    class TestClass2
    {
    public:
        const std::vector<int> &get_value() const noexcept
        {
            return value_;
        }

    public:
        VT_READONLY_PROPERTY(const std::vector<int> &, property, &TestClass2::get_value);

    private:
        std::vector<int> value_;
    };

    TestClass2 test_class;

    std::vector<int> vec{ 1, 2, 3, 4, 5 };
    voot::bind(test_class.property, [&vec]() -> const std::vector<int> & {
        return vec;
    });
    auto &v = test_class.property();
    REQUIRE(&vec == &v);
}
