#include "gui/voot_rectangle.hh"

#include <nanovg/nanovg.h>

VOOT_BEGIN_NAMESPACE

void Rectangle::render(NVGcontext *vg) const noexcept
{
    nvgBeginPath(vg);
    nvgRect(vg,
        float(parent_item()->x_abs() + x()),
        float(parent_item()->y_abs() + y()),
        width(),
        height());
    auto &c = color_;
    nvgFillColor(vg, nvgRGBA(c.red, c.green, c.blue, c.alpha));
    nvgFill(vg);
}

VOOT_END_NAMESPACE
