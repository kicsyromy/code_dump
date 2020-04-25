#include <catch2/catch.hpp>

#define private public

#include "core/voot_application.hh"
#include "voot_application.cc"

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
        REQUIRE(VOOT_APPLICATION() != nullptr);
    }
    REQUIRE(VOOT_APPLICATION() == nullptr);
}

TEST_CASE("Application::register_event_handler(EventCallback, void *, int)", "[application]")
{
    voot::Application application;

    voot::Application::EventCallback cb = [](voot::Event *event, void *user_data) -> bool {
        static_cast<void>(event);
        static_cast<void>(user_data);
        return true;
    };
    application.register_event_handler(voot::EventType::User, cb, nullptr);

    REQUIRE(application.clients_[std::size_t(voot::EventType::User)].size() == 1);

    auto &[priority, callback, user_data] =
        application.clients_[std::size_t(voot::EventType::User)][0];
    REQUIRE(priority == 0);
    REQUIRE(callback == cb);
    REQUIRE(user_data == nullptr);
}

TEST_CASE("Application::post_event(Event *)", "[application]")
{
    [[maybe_unused]] voot::Logger logger;

    class TestEvent : public voot::EventBase<TestEvent>
    {
        DECLARE_EVENT(User, voot::EventCategory::EventCategoryUser);
    };

    struct Context
    {
        voot::Application &app;
        voot::Event *ev;
    };

    voot::Application application;
    auto test_event = new TestEvent();

    voot::Application::EventCallback cb = [](voot::Event *event, void *user_data) -> bool {
        auto *context = static_cast<Context *>(user_data);
        REQUIRE(context->ev == event);
        context->app.quit_ = true;
        return true;
    };

    Context ctx{ application, test_event };
    application.register_event_handler(voot::EventType::User, cb, &ctx);
    application.post_event(test_event);

    application.exec();
}
