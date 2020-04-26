#include "gui/voot_rectangle.hh"

#include <nanovg/nanovg.h>

VOOT_BEGIN_NAMESPACE

void Rectangle::render(NVGcontext *vg, Rectangle *self) noexcept
{
    nvgBeginPath(vg);
    nvgRect(vg, float(self->parent()->width() - 140), 100, 120, 30);
    nvgFillColor(vg, nvgRGBA(0, 0, 255, 255));
    nvgFill(vg);
}

VOOT_END_NAMESPACE
