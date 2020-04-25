#pragma once

#include "voot_global.hh"
#include "events/voot_event.hh"
#include "events/voot_key_events.hh"
#include "events/voot_mouse_events.hh"

#include "gsl/gsl"

#include <array>

VOOT_BEGIN_NAMESPACE

class RenderEvent : public EventBase<RenderEvent>
{
    DECLARE_EVENT(Render, EventCategoryRender);

public:
    constexpr RenderEvent(float elapsed_seconds,
        const std::array<std::uint8_t, KEY_CODE_COUNT> &key_states,
        const std::array<std::uint8_t, MOUSE_BUTTON_COUNT> &mouse_button_states) noexcept
      : elapsed_seconds_{ elapsed_seconds }
      , key_states_{ key_states }
      , mouse_button_states_{ mouse_button_states }
    {}

public:
    constexpr float elapsed_seconds() const noexcept
    {
        return elapsed_seconds_;
    }

    constexpr bool key_pressed(KeyCode key) const noexcept
    {
        return gsl::at(key_states_, std::size_t(key)) != 0;
    }

    constexpr bool mouse_button_pressed(MouseButton button) const noexcept
    {
        return gsl::at(mouse_button_states_, std::size_t(button)) != 0;
    }

private:
    float elapsed_seconds_;
    const std::array<std::uint8_t, KEY_CODE_COUNT> &key_states_;
    const std::array<std::uint8_t, MOUSE_BUTTON_COUNT> &mouse_button_states_;
};

VOOT_END_NAMESPACE
