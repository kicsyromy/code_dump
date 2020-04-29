#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "core/voot_application.hh"
#include "voot_application.cc"

class TestEvent : public voot::EventBase<TestEvent>
{
    DECLARE_EVENT(User, voot::EventCategory::EventCategoryUser);
};

struct EventTester
{
    bool test_event_handler(int i, TestEvent *ev) noexcept
    {
        event_triggered = true;

        id = i;
        event = ev;

        VT_APPLICATION()->quit();

        return true;
    }

    bool event_triggered{ false };

    int id{ -1 };
    voot::Event *event{ nullptr };
};

TEST_CASE("Application creation and destruction", "[application]")
{
    {
        voot::Application application;
        REQUIRE(g_app_instance != nullptr);
    }
    REQUIRE(g_app_instance == nullptr);
}

TEST_CASE("Application::instance()", "[application]")
{
    {
        voot::Application application;
        REQUIRE(VT_APPLICATION() != nullptr);
    }
    REQUIRE(VT_APPLICATION() == nullptr);
}

TEST_CASE("Application::register_event_handler(EventCallback, void *, int)", "[application]")
{
    voot::Application application;

    EventTester tester;
    application.register_event_handler<TestEvent, &EventTester::test_event_handler>(&tester);

    REQUIRE(application.clients_[std::size_t(voot::EventType::User)].size() == 1);

    auto &client = application.clients_[std::size_t(voot::EventType::User)][0];
    REQUIRE(client.id_ == -1);
    REQUIRE(client.priority_ == 0);
    REQUIRE(client.callback_data_ != nullptr);

    TestEvent testEvent;
    client.callback_(-1, &testEvent, &tester);
    REQUIRE(tester.event_triggered == true);
}

TEST_CASE("Application::post_event(Event *)", "[application]")
{
    voot::Application application;

    auto *test_event = new TestEvent;
    EventTester event_tester;

    application.register_event_handler<TestEvent, &EventTester::test_event_handler>(&event_tester,
        0);
    application.post_event(test_event);

    application.exec();

    REQUIRE(event_tester.event_triggered == true);
    REQUIRE(event_tester.id == 0);
    REQUIRE(event_tester.event == test_event);
}
