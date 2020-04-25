#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"

VOOT_BEGIN_NAMESPACE

class KeyEvent
{
public:
    static constexpr std::uint32_t EVENT_CATEGORIES{ EventCategoryInput | EventCategoryKeyboard };

public:
    constexpr KeyEvent(int key) noexcept
      : key_{ key }
    {}

public:
    constexpr int key() const
    {
        return key_;
    }

protected:
    int key_;
};

class KeyPressEvent
  : public KeyEvent
  , public EventBase<KeyPressEvent>
{
    DECLARE_EVENT(KeyPressed, KeyEvent::EVENT_CATEGORIES);

public:
    constexpr KeyPressEvent(int key) noexcept
      : KeyEvent{ key }
      , EventBase<KeyPressEvent>{}
    {}
};

class KeyReleaseEvent
  : public KeyEvent
  , public EventBase<KeyReleaseEvent>
{
    DECLARE_EVENT(KeyReleased, KeyEvent::EVENT_CATEGORIES);

public:
    constexpr KeyReleaseEvent(int key) noexcept
      : KeyEvent{ key }
      , EventBase<KeyReleaseEvent>{}
    {}
};

VOOT_END_NAMESPACE
