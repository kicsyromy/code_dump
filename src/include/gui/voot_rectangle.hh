#pragma once

#include "voot_global.hh"
#include "gui/voot_item.hh"

VT_BEGIN_NAMESPACE

class Rectangle : public ItemBase<Rectangle>
{
    VT_ITEM

public:
    Rectangle()
    {
        set_mouse_event_filter(MouseEventFilterButton);
    }

public:
    constexpr void set_color(std::uint8_t r,
        std::uint8_t g,
        std::uint8_t b,
        std::uint8_t a = 0xff) noexcept
    {
        color_ = { r, g, b, a };
    }

public:
    void render(NVGcontext *vg) const noexcept;

private:
    struct
    {
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
        std::uint8_t alpha;
    } color_;
};

VT_END_NAMESPACE
