#include "gui/voot_rectangle.hh"

#include <nanovg/nanovg.h>

VT_BEGIN_NAMESPACE

void Rectangle::render(NVGcontext *vg) const noexcept
{
    nvgBeginPath(vg);
    nvgRect(vg, float(x_abs()), float(y_abs()), width(), height());
    auto &c = color_;
    if (focus())
    {
        nvgFillColor(vg, nvgRGBA(c.red / 2, c.green / 2, c.blue / 2, c.alpha / 2));
    }
    else
    {
        nvgFillColor(vg, nvgRGBA(c.red, c.green, c.blue, c.alpha));
    }
    nvgFill(vg);
}

VT_END_NAMESPACE
