#include "gui/voot_rectangle.hh"

#include <nanovg/nanovg.h>

VOOT_BEGIN_NAMESPACE

void Rectangle::render(NVGcontext *vg, Rectangle *self) noexcept
{
    nvgBeginPath(vg);
    nvgRect(vg, float(self->x_), float(self->y_), self->width(), self->height());
    auto &c = self->color_;
    nvgFillColor(vg, nvgRGBA(c.red, c.green, c.blue, c.alpha));
    nvgFill(vg);
}

VOOT_END_NAMESPACE
