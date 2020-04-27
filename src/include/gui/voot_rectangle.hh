#pragma once

#include "voot_global.hh"
#include "gui/voot_item.hh"

VOOT_BEGIN_NAMESPACE

class Rectangle : public ItemBase<Rectangle>
{
    VT_ITEM

public:
    constexpr void set_color(std::uint8_t r,
        std::uint8_t g,
        std::uint8_t b,
        std::uint8_t a = 0xff) noexcept
    {
        color_ = { r, g, b, a };
    }

public:
    static void render(NVGcontext *vg, const void *instance) noexcept;

private:
    struct
    {
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
        std::uint8_t alpha;
    } color_;
};

VOOT_END_NAMESPACE
