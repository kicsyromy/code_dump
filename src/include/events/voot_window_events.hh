#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"

#include <variant>

VT_BEGIN_NAMESPACE

class WindowShowEvent : public EventBase<WindowShowEvent>
{
    VT_DECLARE_EVENT(WindowShown, EventCategoryWindow);

public:
    constexpr WindowShowEvent() noexcept = default;
};

class WindowHideEvent : public EventBase<WindowHideEvent>
{
    VT_DECLARE_EVENT(WindowHidden, EventCategoryWindow);

public:
    constexpr WindowHideEvent() noexcept = default;
};

class WindowCloseEvent : public EventBase<WindowCloseEvent>
{
    VT_DECLARE_EVENT(WindowClosed, EventCategoryWindow);

public:
    constexpr WindowCloseEvent() noexcept = default;
};

class WindowResizeEvent : public EventBase<WindowResizeEvent>
{
    VT_DECLARE_EVENT(WindowResized, EventCategoryWindow);

public:
    constexpr WindowResizeEvent(std::uint16_t width, std::uint16_t height) noexcept
      : EventBase<WindowResizeEvent>{}
      , width_{ width }
      , height_{ height }
    {}

public:
    constexpr std::pair<std::uint16_t, std::uint16_t> size() const noexcept
    {
        return { width_, height_ };
    }

private:
    std::uint16_t width_;
    std::uint16_t height_;
};

class WindowGainFocusEvent : public EventBase<WindowGainFocusEvent>
{
    VT_DECLARE_EVENT(WindowGainedFocus, EventCategoryWindow);

public:
    constexpr WindowGainFocusEvent() noexcept = default;
};

class WindowLooseFocusEvent : public EventBase<WindowLooseFocusEvent>
{
    VT_DECLARE_EVENT(WindowLostFocus, EventCategoryWindow);

public:
    constexpr WindowLooseFocusEvent() noexcept = default;
};

class WindowMoveEvent : public EventBase<WindowMoveEvent>
{
    VT_DECLARE_EVENT(WindowMoved, EventCategoryWindow);

public:
    constexpr WindowMoveEvent(std::int32_t x, std::int32_t y) noexcept
      : EventBase<WindowMoveEvent>{}
      , x_{ x }
      , y_{ y }
    {}

public:
    constexpr std::pair<std::int32_t, std::int32_t> position() const noexcept
    {
        return { x_, y_ };
    }

private:
    std::int32_t x_;
    std::int32_t y_;
};

using WindowEventVariant = std::variant<std::monostate,
    WindowShowEvent,
    WindowHideEvent,
    WindowCloseEvent,
    WindowResizeEvent,
    WindowGainFocusEvent,
    WindowLooseFocusEvent,
    WindowMoveEvent>;

VT_END_NAMESPACE
