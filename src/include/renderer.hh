#pragma once

#include <functional>

typedef struct NVGcontext NVGcontext;

namespace renderer
{
    void init(std::function<void(NVGcontext *)> &&render_cb) noexcept;
    void destroy() noexcept;
    void run() noexcept;
} // namespace renderer
