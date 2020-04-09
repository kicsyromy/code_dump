#pragma once

#include <functional>

typedef struct NVGcontext NVGcontext;

namespace renderer
{
    void init(std::function<void(NVGcontext *, int, int, float)> &&render_cb) noexcept;
    void destroy() noexcept;
    void run(const std::function<void(int)> &kcb) noexcept;
} // namespace renderer
