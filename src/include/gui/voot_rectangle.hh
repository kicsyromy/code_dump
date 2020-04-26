#pragma once

#include "voot_global.hh"
#include "gui/voot_item.hh"

VOOT_BEGIN_NAMESPACE

class Rectangle : public ItemBase<Rectangle>
{
public:
    static void render(NVGcontext *vg, Rectangle *self) noexcept;
};

VOOT_END_NAMESPACE
