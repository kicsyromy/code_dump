#include "gui/voot_rectangle.hh"

#include <core/SkCanvas.h>

VT_BEGIN_NAMESPACE

void Rectangle::render(SkCanvas *canvas) const noexcept
{
    canvas->save();

    const auto rectangle = SkRect::MakeXYWH(SkIntToScalar(x_abs()),
        SkIntToScalar(y_abs()),
        SkIntToScalar(width()),
        SkIntToScalar(height()));

    auto &c = color_;
    const auto paint = SkPaint(SkColor4f{ float(c.red) / 255,
        float(c.green) / 255,
        float(c.blue) / 255,
        float(c.alpha) / 255 });

    canvas->drawRect(rectangle, paint);

    canvas->restore();
}

VT_END_NAMESPACE
