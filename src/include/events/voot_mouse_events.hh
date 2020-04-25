#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"

VOOT_BEGIN_NAMESPACE

class MouseEvent
{
public:
    static constexpr std::uint32_t EVENT_CATEGORIES{ EventCategoryMouse | EventCategoryInput };

public:
    constexpr MouseEvent(std::int32_t x, std::int32_t y) noexcept
      : x_{ x }
      , y_{ y }
    {}

public:
    constexpr std::pair<std::int32_t, std::int32_t> coordinates() noexcept
    {
        return { x_, y_ };
    }

protected:
    std::int32_t x_{};
    std::int32_t y_{};
};

class MouseMoveEvent
  : public MouseEvent
  , public EventBase<MouseMoveEvent>
{
    DECLARE_EVENT(MouseMoved, MouseEvent::EVENT_CATEGORIES);

public:
    constexpr MouseMoveEvent(int x, int y) noexcept
      : MouseEvent{ x, y }
      , EventBase<MouseMoveEvent>{}
    {}
};

enum class MouseButton
{
    Left,
    Middle,
    Right,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
    Button9,
    Count
};

constexpr std::size_t MOUSE_BUTTON_COUNT{ std::size_t(MouseButton::Count) };

class MouseButtonEvent : public MouseEvent
{
public:
    static constexpr std::uint32_t EVENT_CATEGORIES{ EventCategoryMouse | EventCategoryInput |
                                                     EventCategoryMouseButton };

public:
    constexpr MouseButtonEvent(int x, int y, MouseButton button) noexcept
      : MouseEvent{ x, y }
      , button_{ button }
    {}

public:
    constexpr MouseButton button() const noexcept
    {
        return button_;
    }

protected:
    MouseButton button_;
};

class MouseButtonPressEvent
  : public MouseButtonEvent
  , public EventBase<MouseButtonPressEvent>
{
    DECLARE_EVENT(MouseButtonPressed, MouseButtonEvent::EVENT_CATEGORIES);

public:
    constexpr MouseButtonPressEvent(int x, int y, MouseButton button) noexcept
      : MouseButtonEvent{ x, y, button }
      , EventBase<MouseButtonPressEvent>{}
    {}
};

class MouseButtonReleaseEvent
  : public MouseButtonEvent
  , public EventBase<MouseButtonPressEvent>
{
    DECLARE_EVENT(MouseButtonReleased, MouseButtonEvent::EVENT_CATEGORIES);

public:
    constexpr MouseButtonReleaseEvent(int x, int y, MouseButton button) noexcept
      : MouseButtonEvent{ x, y, button }
      , EventBase<MouseButtonPressEvent>{}
    {}
};

VOOT_END_NAMESPACE
