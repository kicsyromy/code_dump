#pragma once

#include "voot_global.hh"

#include <string_view>
#include <type_traits>

#define DECLARE_EVENT(name, categories)                        \
public:                                                        \
    constexpr static voot::EventType event_type() noexcept     \
    {                                                          \
        return voot::EventType::name;                          \
    }                                                          \
    constexpr static std::uint32_t event_categories() noexcept \
    {                                                          \
        return categories;                                     \
    }                                                          \
    constexpr static const char *event_name() noexcept         \
    {                                                          \
        return #name;                                          \
    }                                                          \
    static_assert(true)

VOOT_BEGIN_NAMESPACE

enum class EventType
{
    None = 0,
    Render,
    WindowShown,
    WindowHidden,
    WindowClosed,
    WindowResized,
    WindowGainedFocus,
    WindowLostFocus,
    WindowMoved,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    User,
    Count
};

constexpr std::size_t EVENT_TYPE_COUNT{ std::size_t(EventType::Count) };

enum EventCategory : std::uint32_t
{
    None = 0,
    EventCategoryRender = 1 << 0,
    EventCategoryWindow = 1 << 1,
    EventCategoryInput = 1 << 2,
    EventCategoryKeyboard = 1 << 3,
    EventCategoryMouse = 1 << 4,
    EventCategoryMouseButton = 1 << 5,
    EventCategoryReserved1 = 1 << 6,
    EventCategoryReserved2 = 1 << 7,
    EventCategoryUser = 1 << 8
};

class Event
{
public:
    using fn_static_event_type_t = EventType (*)();
    using fn_static_event_categories_t = std::uint32_t (*)();
    using fn_static_event_name_t = const char *(*)();

public:
    constexpr Event(fn_static_event_type_t fn_static_event_type,
        fn_static_event_categories_t fn_static_event_categories,
        fn_static_event_name_t fn_static_event_name = nullptr) noexcept
      : static_event_type_{ fn_static_event_type }
      , static_event_categories_{ fn_static_event_categories }
      , static_event_name_{ fn_static_event_name }

    {}

public:
    constexpr EventType event_type() const
    {
        return static_event_type_();
    }

    constexpr std::uint32_t event_categories() const
    {
        return static_event_categories_();
    }

    constexpr const char *event_name() const
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
    constexpr EventBase() noexcept
      : Event{ &ChildEvent::event_type, &ChildEvent::event_categories, &ChildEvent::event_name }
    {}
};

VOOT_END_NAMESPACE
