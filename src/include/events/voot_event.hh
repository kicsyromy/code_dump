#pragma once

#include "voot_global.hh"

#include <string_view>
#include <type_traits>

#define DECLARE_EVENT(name, categories)              \
public:                                              \
    static voot::EventType event_type() noexcept     \
    {                                                \
        return voot::EventType::name;                \
    }                                                \
    static std::uint32_t event_categories() noexcept \
    {                                                \
        return categories;                           \
    }                                                \
    static const char *event_name() noexcept         \
    {                                                \
        return #name;                                \
    }                                                \
    static_assert(true)

VOOT_BEGIN_NAMESPACE

enum class EventType
{
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    User
};

enum EventCategory
{
    None = 0,
    EventCategoryWindow = 1 << 0,
    EventCategoryInput = 1 << 1,
    EventCategoryKeyboard = 1 << 2,
    EventCategoryMouse = 1 << 3,
    EventCategoryMouseButton = 1 << 4,
    EventCategoryReserved1 = 1 << 5,
    EventCategoryReserved2 = 1 << 6,
    EventCategoryUser = 1 << 7
};

class Event
{
public:
    using fn_static_event_type_t = EventType (*)();
    using fn_static_event_categories_t = std::uint32_t (*)();
    using fn_static_event_name_t = const char *(*)();

public:
    Event(fn_static_event_type_t fn_static_event_type,
        fn_static_event_categories_t fn_static_event_categories,
        fn_static_event_name_t fn_static_event_name = nullptr) noexcept
      : static_event_type_{ fn_static_event_type }
      , static_event_categories_{ fn_static_event_categories }
      , static_event_name_{ fn_static_event_name }

    {}

public:
    EventType event_type() const
    {
        return static_event_type_();
    }

    std::uint32_t event_categories() const
    {
        return static_event_categories_();
    }

    const char *event_name() const
    {
        return static_event_name_();
    }

private:
    fn_static_event_type_t static_event_type_;
    fn_static_event_categories_t static_event_categories_;
    fn_static_event_name_t static_event_name_;
};

template<typename ChildEvent> class EventBase : public Event
{
public:
    EventBase()
      : Event{ &ChildEvent::event_type, &ChildEvent::event_categories, &ChildEvent::event_name }
    {}
};

VOOT_END_NAMESPACE
